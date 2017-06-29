#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include<thread>

using namespace std;

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

void SORT(string quote_file, vector<Order> &orders) {
	ofstream fout("report.txt");
	map<string, map<string, pair<int, double>>> deal_order; // exchange->symbol-><quantity, total cost>;
	map<string, map<string, pair<int, double>>> book_ask; // exchange->symbol-><quantity, price>;
	map<string, map<string, pair<int, double>>> book_bid; // exchange->symbol-><quantity, price>;
	ifstream in_q(quote_file);
	string quote;
	string exchange;
	string side;
	string symbol;
	string size;
	string price;
	getline(in_q, quote);
	//************ Part 1: The aggresive take ************* 
	while (getline(in_q, quote)) {
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
			if (side == "Bid") {
				// if read quote satisfies the demand of some order, process it
				if ((orders[i].side == "Sell") && (quote_price >= orders[i].price)
					&&(orders[i].filled < orders[i].quantity) && (symbol == orders[i].symbol)) {
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
	}

	// write the result of aggresive strategy according to orders to the created text file 
	for (map<string, map<string, pair<int, double>>>::iterator ex = deal_order.begin(); ex != deal_order.end(); ex++) {
		fout << "For " << ex->first << " :" << endl ;
		for (map<string, pair<int, double>>::iterator sy = ex->second.begin(); sy != ex->second.end(); sy++) {
			fout << sy->first << ": " << endl;
			fout << "Dealed quanty is " << sy->second.first << "   ";
			fout << "Average price is " << sy->second.second / sy->second.first << endl << endl;
		}
	}

	fout << "-------------------------" << endl;

	// ************** Part 2: Passive pacement ***********
	for (int i = 0; i < orders.size(); i++) {
		fout << endl << "For order " << i + 1 << " : " << endl;
		fout << "Total quantity is " << orders[i].filled << "   ";
		fout << "Average price is " << orders[i].cost / orders[i].filled << endl;
		if (orders[i].filled < orders[i].quantity) {
			// for orders not being totally filled, divide the remaining volume to situable exchanges
			int left = orders[i].quantity - orders[i].filled;
			fout << "Left volume is " << left << endl;
			fout << "Divided to: ";
			// Note that we caculate the diveded volume to each exchange only refering to the information
			// of top level on the bid and ask books
			if (orders[i].side == "Buy") {
				int total = 0;
				for (map<string, map<string, pair<int, double>>>::iterator it = book_ask.begin(); it != book_ask.end(); it++) {
					total += it->second[orders[i].symbol].first;
				}
				for (map<string, map<string, pair<int, double>>>::iterator it = book_ask.begin(); it != book_ask.end(); it++) {
					fout << it->first << ": " << int(double(left)*it->second[orders[i].symbol].first/total) << "  ";
				}
				fout << endl;
			}
			if (orders[i].side == "Sell") {
				int total = 0;
				for (map<string, map<string, pair<int, double>>>::iterator it = book_bid.begin(); it != book_bid.end(); it++) {
					total += it->second[orders[i].symbol].first;
				}
				for (map<string, map<string, pair<int, double>>>::iterator it = book_bid.begin(); it != book_bid.end(); it++) {
					fout << it->first << ": " << int(double(left)*it->second[orders[i].symbol].first / total) << "  ";
				}
				fout << endl;
			}
		}
	}
	
	fout.close();
}

int main() {
	ifstream in_order("Orders.csv");
	vector<Order> orders;
	string line;
	string temp;
	string side;
	string symbol;
	string price;
	string quantity;
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
	string quote_file = "Quotes.csv";
	SORT(quote_file, orders);
}