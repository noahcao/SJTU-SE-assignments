/**********************************************
Overview:

Program to realize SORT on data, with consists of aggresive take and passive placement, and in
a single thread edition. Two text files would be output, which are 'final_result_st.txt' and
'real_time_status.txt'. The former records the final result of orders and exchanges traditngs of
aggresive take, and the latter one records the real-time status of both orders and level1 book.

PS.after real test on a personal computer, time used to process each quote is about 0.1 ms 
	(time for writing real_time information into file is contained, otherwise, time used is about
	3e-6 s).
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

ofstream status("real_time_status_mt.txt");

LARGE_INTEGER litmp;				// variants defined to record the time used of each quote processing
LONGLONG QPart1, QPart2;			// same as above
double dfMinus, dfFreq, dfTim;		// same as above

double total_time_quote = 0;		// record the total time used to process quotes

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
vector<Order> orders = order_generalize(order_file); // generalize the order book through given order file;

void status_update() {
	status.setf(ios::left);
	status << "Level1 Books Updated!" << endl;
	status << "***************** Ask Book ***************** " << endl;
	for (map<string, map<string, pair<int, double>>>::iterator ex = book_ask.begin(); ex != book_ask.end(); ex++) {
		status.setf(ios::left);
		status << setw(10) << ex->first << ": ";
		for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
			status << "  " << setw(6) << sy->first << setw(5) << sy->second.first << "@" << setw(8) << sy->second.second;
		}
		status << endl;
	}
	status << endl << "***************** Bid Book ***************** " << endl;
	for (map<string, map<string, pair<int, double>>>::iterator ex = book_bid.begin(); ex != book_bid.end(); ex++) {
		status.setf(ios::left);
		status << setw(10) << ex->first << ": ";;
		for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
			status << "  " << setw(6) << sy->first << setw(5) << sy->second.first << "@" << setw(8) << sy->second.second;
		}
		status << endl;
	}
	for (int i = 0; i < orders.size(); i++) {
		status << endl << "Order " << i + 1 << setw(4) << ": ";
		status << "Total quantity: " << orders[i].filled << "   ";
		if (orders[i].filled < orders[i].quantity) {
			// for orders not being totally filled, divide the remaining volume to situable exchanges
			int left = orders[i].quantity - orders[i].filled;
			status << "Left volume: " << setw(8) << left;
			status << "Divided to: ";
			// Note that we caculate the diveded volume to each exchange only refering to the information
			// of top level on the bid and ask books
			if (orders[i].side == "Buy") {
				int total = 0;
				for (map<string, map<string, pair<int, double>>>::iterator it = book_ask.begin(); it != book_ask.end(); it++) {
					total += it->second[orders[i].symbol].first;
				}
				for (map<string, map<string, pair<int, double>>>::iterator it = book_ask.begin(); it != book_ask.end(); it++) {
					status << it->first << "@" << int(double(left)*it->second[orders[i].symbol].first / total) << "  ";
				}
			}
			if (orders[i].side == "Sell") {
				int total = 0;
				for (map<string, map<string, pair<int, double>>>::iterator it = book_bid.begin(); it != book_bid.end(); it++) {
					total += it->second[orders[i].symbol].first;
				}
				for (map<string, map<string, pair<int, double>>>::iterator it = book_bid.begin(); it != book_bid.end(); it++) {
					status << it->first << "@" << int(double(left)*it->second[orders[i].symbol].first / total) << "  ";
				}
			}
		}
		status << endl;
	}
	status << endl;
}

void result_write() {
	ofstream fout("final_report_mt.txt");		// output textfile

	fout << "All quotes finished ! Total time to process qutoes is: " << total_time_quote << "s" << endl << endl;
	fout << "The result of aggressive take: " << endl << endl;

	fout.setf(ios::left);
	for (map<string, map<string, pair<int, double>>>::iterator ex = deal_order.begin(); ex != deal_order.end(); ex++) {
		fout << "*************** " << ex->first << " ****************" << endl << endl;
		for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
			fout << setw(5) << sy->first << ": ";
			fout << "Dealed quanty@" << sy->second.first << "   ";
			fout << "Average price@" << sy->second.second / sy->second.first << endl << endl;
		}
	}

	fout << "-------------------------" << endl;

	for (int i = 0; i < orders.size(); i++) {
		fout << endl << "Order " << i + 1 << setw(4) << ": ";
		fout << "Total quantity: " << orders[i].filled << "   ";
		fout << "Average price: " << orders[i].cost / orders[i].filled << endl;
	}
	fout.close();
}

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
		
		status << "                  Order status: " << endl;
		status.setf(ios::left);
		// print the updated information of orders
		for (int i = 0; i < orders.size(); i++) {
			status << "order " << i + 1 << " :";
			status << "total quantity: " << setw(8) << orders[i].quantity;
			status << "  filled: " << setw(8) << orders[i].filled;
			status << "  left: " << setw(8) << orders[i].quantity - orders[i].filled << endl;
		}
		status << endl;
		// print the updated information of books

		if (!total_taken) status_update();			// update status of order and book

		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;				// timing ends
		dfMinus = (double)(QPart2 - QPart1);
		dfTim = dfMinus / dfFreq;
		status << "A quote finished! Time used: " << dfTim * 1000000 << "¦Ìs" << endl << endl;
		total_time_quote += dfTim;
	}
	result_write();
}

void thread_main() {
	/* thread to get command input by user and show the real-time status or orders or level1 books
	 * note that though real-time information in tracked, the processing of quotes needs only about
	 * 0.02s, so when user input the command, that's highly likely that the ouput result is all same
	 * which is the final status of orders and level1 books*/
	while (true) {
		string command;
		cout << "Command options to get real-time information('O' for orders, 'B' for level1 books, 'Q' to quit):";
		if (cin >> command) {
			if (command == "O") {
				for (int i = 0; i < orders.size(); i++) {
					cout << endl << "Order " << i + 1 << setw(4) << ": ";
					cout << "Total quantity: " << orders[i].filled << "   ";
					cout << "Average price: " << orders[i].cost / orders[i].filled << endl;
				}
				cout << endl;
			}
			if (command == "B") {
				cout << endl << "***************** Ask Book ***************** " << endl;
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
			}
			if (command == "Q") { return; }
		}
	}
}

int main() {
	InitializeCriticalSection(&g_CS);
	string quote_file = "Quotes.csv";
	thread task_quote(thread_quote, quote_file);	// thread to process quotes
	thread task_main(thread_main);

	task_main.join();
	task_quote.join();

	system("pause");
}