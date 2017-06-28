/**********************************************
Overview:
In the program, combined strategies are used to process the given orders and quotes, fundamental idea
is to firstly try aggressive take, after which if a quote has not been all taken, do the passive placement
then. Note that, during each quote processing, the time used will be print real-time, and if passive placement
is performed, the information of orders and level1 books would also be updated and printed. Once all quoted
have already been processed, the summary information would be output to a textfile, which contains the 
dealed quantity and average price of each symbol in different exchange, and the final status of each order, 
togather with the filled quantit, left quantity and average price of dealed part.

PS.after real test on a personal computer, time used to process each quote is about 5e-6 s.
***********************************************/

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include<thread>
#include<afxmt.h>
#include<iomanip>

using namespace std;

LARGE_INTEGER litmp;				// variants defined to record the time used of each quote processing
LONGLONG QPart1, QPart2;			// same as above
double dfMinus, dfFreq, dfTim;		// same as above

double total_time_quote = 0;		// record the total time used to process quotes

CEvent status_update;
map<string, map<string, pair<int, double>>> deal_order;		// exchange->symbol-><quantity, total cost>;
map<string, map<string, pair<int, double>>> book_ask;		// exchange->symbol-><quantity, price>;
map<string, map<string, pair<int, double>>> book_bid;		// exchange->symbol-><quantity, price>;

bool quote_finish = false;
CRITICAL_SECTION g_CS;				// define a critical section to ensure printing information in order

struct Order {
	string symbol;
	string side;
	double price;
	int quantity;
	int filled;
	double cost;
	Order(string Symbol, string Side, double Price, int Quantity) {
		symbol = Symbol;
		side = Side;
		price = Price;
		quantity = Quantity;
		filled = 0;
		cost = 0;
	}
};

vector<Order> order_generalize(string order_file) {
	// function to generalize the information of orders into of vector of orders
	ifstream in_order(order_file);
	vector<Order> orders;
	string line, temp, side, symbol, price, quantity;
	getline(in_order, line, '\r');
	while (getline(in_order, line, '\r')) {
		stringstream buffer;
		buffer << line;
		getline(buffer, temp, ',');
		getline(buffer, side, ',');
		getline(buffer, symbol, ',');
		getline(buffer, price, ',');
		getline(buffer, quantity, ',');
		orders.push_back(Order(symbol, side, atof(price.c_str()), atoi(quantity.c_str())));
	}
	return orders;
}

string order_file = "Orders.csv";
vector<Order> orders;

void thread_quote(string quote_file) {
	ifstream in_q(quote_file);
	string quote, exchange, side, symbol, size, price;
	getline(in_q, quote);
	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;
	while (getline(in_q, quote)) {
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart; 				// timing begins

		stringstream buffer;
		buffer << quote;
		getline(buffer, exchange, ',');
		getline(buffer, side, ',');
		getline(buffer, symbol, ',');
		getline(buffer, size, ',');
		getline(buffer, price, ',');
		bool total_taken = false;					// indicate whether all share of the quote is taken 
		int quote_size = atoi(size.c_str());
		double quote_price = atof(price.c_str());
		for (int i = 0; i < orders.size(); i++) {
			// *********** Step 1: Try to aggresive take ************
			if (side == "Bid") {
				// if read quote satisfies the demand of some order, process it
				if ((orders[i].side == "Sell") && (quote_price >= orders[i].price)
					&& (orders[i].filled < orders[i].quantity) && (symbol == orders[i].symbol)) {
					if (orders[i].filled + quote_size > orders[i].quantity) {
						// case in which the quote size only partly taken by an order
						int deal_size = orders[i].quantity - orders[i].filled;
						quote_size -= deal_size;
						orders[i].filled += deal_size;
						orders[i].cost += quote_price*deal_size;
						deal_order[exchange][symbol].first += deal_size;
						deal_order[exchange][symbol].second += deal_size*quote_price;
					}
					else {
						// case in which the quote all taken 
						orders[i].filled += quote_size;
						orders[i].cost += quote_price*quote_size;
						deal_order[exchange][symbol].first += quote_size;
						deal_order[exchange][symbol].second += quote_size*quote_price;
						total_taken = true;
						break;
					}
				}
			}
			if (side == "Ask") {
				if ((orders[i].side == "Buy") && (quote_price <= orders[i].price)
					&& (orders[i].filled<orders[i].quantity) && (symbol == orders[i].symbol)) {
					if (orders[i].filled + quote_size > orders[i].quantity) {
						int deal_size = orders[i].quantity - orders[i].filled;
						quote_size -= deal_size;
						orders[i].filled += deal_size;
						orders[i].cost += quote_price*deal_size;
						deal_order[exchange][symbol].first += deal_size;
						deal_order[exchange][symbol].second += deal_size*quote_price;
					}
					else {
						orders[i].filled += quote_size;
						orders[i].cost += quote_price*quote_size;
						deal_order[exchange][symbol].first += quote_size;
						deal_order[exchange][symbol].second += quote_size*quote_price;
						total_taken = true;
						break;
					}
				}
			}
		}
		// ****** Step 2: when the quote not having been all taken, do passive placement ****
		if (!total_taken) {
			// when quote share not all taken into deal, do further processing
			if (side == "Bid") {
				// update the bid order book
				book_bid[exchange][symbol].first = quote_size;
				book_bid[exchange][symbol].second = quote_price;
			}
			if (side == "Ask") {
				// update the ask order book
				book_ask[exchange][symbol].first = quote_size;
				book_ask[exchange][symbol].second = quote_price;
			}
		}

		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;				// timing ends
		dfMinus = (double)(QPart2 - QPart1);
		dfTim = dfMinus / dfFreq;
		
		EnterCriticalSection(&g_CS);
		cout << endl << "time used: " << dfTim*1000000 << "¦Ìs" << endl;
		LeaveCriticalSection(&g_CS);
		total_time_quote += dfTim;
		if (!total_taken) status_update.SetEvent();			// send singal to main thread to update status of order and book
	}
	quote_finish = true;
}

void thread_main() {
	orders = order_generalize(order_file);					// generalize the order book through given order file
	while (WaitForSingleObject(status_update, INFINITE) == WAIT_OBJECT_0) {
		EnterCriticalSection(&g_CS);
		cout << "                  Order status: " << endl;
		cout.setf(ios::left);
		// print the updated information of orders
		for (int i = 0; i < orders.size(); i++) {
			cout << "order " << i + 1 << " :";
			cout << "total quantity: " << setw(8) << orders[i].quantity;
			cout << "  filled: " << setw(8) << orders[i].filled;
			cout << "  left: " << setw(8) << orders[i].quantity - orders[i].filled << endl;
		}
		// print the updated information of books
		cout << endl << "                  Level1 Books " << endl << endl;
		cout <<  "***************** Ask Book ***************** " << endl;
		for (map<string, map<string, pair<int, double>>>::iterator ex = book_ask.begin(); ex != book_ask.end(); ex++) {
			cout.setf(ios::left);
			cout << setw(10) << ex->first << ": ";
			for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
				cout << "  " << setw(6) << sy->first << setw(5) << sy->second.first << "@" << setw(8) << sy->second.second;
			}
			cout << endl;
		}
		cout << endl << "***************** Bid Book ***************** " << endl;
		for (map<string, map<string, pair<int, double>>>::iterator ex = book_bid.begin(); ex != book_bid.end(); ex++) {
			cout.setf(ios::left);
			cout << setw(10) << ex->first << ": ";;
			for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
				cout << "  " << setw(6) << sy->first << setw(5) << sy->second.first << "@" << setw(8) << sy->second.second;
			}
			cout << endl;
		}
		cout << endl;
		LeaveCriticalSection(&g_CS);
	}
}

void result_write() {
	ofstream fout("report.txt");		// output textfile
	// write the result of aggresive strategy according to orders to the created text file 
	fout.setf(ios::left);
	for (map<string, map<string, pair<int, double>>>::iterator ex = deal_order.begin(); ex != deal_order.end(); ex++) {
		fout << "                  " << ex->first << endl;
		for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
			fout << setw(5) << sy->first << ": ";
			fout << "Dealed quanty@" << sy->second.first << "   ";
			fout << "Average price@" << sy->second.second / sy->second.first << endl;
		}
	}

	fout << "-------------------------" << endl;

	for (int i = 0; i < orders.size(); i++) {
		fout << endl << "Order " << i + 1 << setw(4) << ": ";
		fout << "Total quantity: " << orders[i].filled << "   ";
		fout << "Average price: " << orders[i].cost / orders[i].filled << endl;
		if (orders[i].filled < orders[i].quantity) {
			// for orders not being totally filled, divide the remaining volume to situable exchanges
			int left = orders[i].quantity - orders[i].filled;
			fout << "Left volume: " << setw(8) << left;
			fout << "Divided to: ";
			// Note that we caculate the diveded volume to each exchange only refering to the information
			// of top level on the bid and ask books
			if (orders[i].side == "Buy") {
				int total = 0;
				for (map<string, map<string, pair<int, double>>>::iterator it = book_ask.begin(); it != book_ask.end(); it++) {
					total += it->second[orders[i].symbol].first;
				}
				for (map<string, map<string, pair<int, double>>>::iterator it = book_ask.begin(); it != book_ask.end(); it++) {
					fout << it->first << "@" << int(double(left)*it->second[orders[i].symbol].first / total) << "  ";
				}
				fout << endl;
			}
			if (orders[i].side == "Sell") {
				int total = 0;
				for (map<string, map<string, pair<int, double>>>::iterator it = book_bid.begin(); it != book_bid.end(); it++) {
					total += it->second[orders[i].symbol].first;
				}
				for (map<string, map<string, pair<int, double>>>::iterator it = book_bid.begin(); it != book_bid.end(); it++) {
					fout << it->first << "@" << int(double(left)*it->second[orders[i].symbol].first / total) << "  ";
				}
				fout << endl;
			}
		}
	}
	fout.close();
}

int main() {
	InitializeCriticalSection(&g_CS);
	string quote_file = "Quotes.csv";
	thread task_quote(thread_quote, quote_file);	// thread to process quotes
	thread task_main(thread_main);					// thread to update the information of orders and books
	task_quote.join();
	if(!quote_finish) task_main.join();				// while quotes not all processed, keep the thread active
	if (quote_finish) {								// once all quotes processed, print the final result
		result_write();
		cout << endl << "Total time to process qutoes is: " << total_time_quote << "s" << endl;
	}
	system("pause");
}