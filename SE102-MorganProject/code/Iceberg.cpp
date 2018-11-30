/******************************
In this program, we implement a method to split a order with large volume into
many small ones, and the share of each sub order is determined by the history data.
Thus, we process history record and calculate the average price and quantity ratio
during different time slots which would be reference for us to do further processing 
on the large original order.
******************************/

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<map>
#include<iomanip>

using namespace std;

int interval = 30;
// function to process a timestamp to a timeindex, default interval time between
// neighbor indexes is 30 minutes
int timeslice(string time) { 
	int time_min = 9 * 3600;						// business begins at 9:00 a.m
	stringstream s_stream;
	string hours, minutes, seconds;
	s_stream << time;
	getline(s_stream, hours, ':');
	getline(s_stream, minutes, ':');
	getline(s_stream, seconds);
	int time_in_sec = stoi(hours) * 3600 + stoi(minutes) * 60 + stoi(seconds);
	int timeindex = (time_in_sec - time_min) / 1800;
	return timeindex;
}

map<string, map<int, pair<double, int>>> history_analysis(string filename) {
	// return map: date->timeindex-><cost, quantity>
	map<string, map<int, pair<double, int>>> history;
	ifstream in_file(filename);
	string date, time, price, quantity;
	string line;
	int pre_quantity =0 ;
	int pre_index = 0;
	while (getline(in_file, line)) {
		if (line[0] == '*') {			// in original file, "*******" seperate data of different days away
			pre_index = 0;
			pre_quantity = 0;
		}
		if (line[0] == '2') {			
			stringstream buffer;
			buffer << line;
			// split the read string into four different parts
			getline(buffer, date, ' ');
			getline(buffer, time, ',');
			getline(buffer, price, ',');
			getline(buffer, quantity);

			// note that timeindex means an index of time slot in order
			int timeindex = timeslice(time);
			int new_quantity = stoi(quantity);
			double new_cost = (new_quantity - pre_quantity)*stof(price);
			if (timeindex == pre_index) {
				history[date][timeindex].first += new_cost;
				history[date][timeindex].second += new_quantity - pre_quantity;
			}
			else {
				// For fist record of each time slot should be processed specially
				pre_index = timeindex;
				history[date][timeindex].first = 0;
				history[date][timeindex].second = 0;
			}
			pre_quantity = new_quantity;
		}
	}
	return history;
}

map<int, pair<double, double>> predict(map<string, map<int, pair<double, int>>> history) {
	// output...map that: timeindex->ratio(%)->average price
	int alltime_quantity = 0;
	map<int, pair<double, double>> predict;
	int index_range = 7 * 60 / interval;				// during a whole day, only 7 hours are possible to do business
	for (int i = 0; i < index_range; i++) {
		double total_cost = 0;
		int total_quantity = 0;
		for (map<string, map<int, pair<double, int>>>::iterator it = history.begin(); it != history.end(); it++) {
			total_cost += it->second[i].first;
			total_quantity += it->second[i].second;
		}
		alltime_quantity += total_quantity;
		double average_price = total_cost / total_quantity;
		predict[i].first = total_quantity;
		predict[i].second = average_price;
	}
	for (int i = 0; i < index_range; i++) {
		predict[i].first = predict[i].first / alltime_quantity;		// calculate the ratio of each time segmentaion
	}
	return predict;
}

int main() {
	string filename = "600690.iceberg.csv";
	ofstream fout("split_result.txt");
	// data -> timeindex -> <total cost, total quantity>
	map<string, map<int, pair<double, int>>> history = history_analysis(filename);

	int order_volume;
	cout << "Please enter the order volume: ";
	cin >> order_volume;
	map<int, pair<double, double>> iceberg_slice = predict(history);		// map to guid the slice to order volume

	fout << endl << "****** Slice Order ******" << endl << endl;

	double total_cost = 0;					// the total cost during a day

	// code below to print the final result of the iceberg processing
	for (int i = 0; i < 7 * 60 / interval; i++) {
		double start_time = 9 + i*0.5;
		double end_time = 9 + (i + 1)*0.5;
		double average_price = iceberg_slice[i].second;
		int volum = int(iceberg_slice[i].first*order_volume);
		fout << setw(4) << start_time  << "h --- " << setw(4) << end_time <<  setw(1) << "h";
		fout << "  send " << volum << " (" << setiosflags(ios::right) << setw(8) << iceberg_slice[i].first * 100 << "% )";
		fout << "  predicted average price is " << iceberg_slice[i].second << endl;
		total_cost += iceberg_slice[i].second*volum;
	}

	// print the summary information of the order slice
	fout << endl << "Total cost is " << total_cost << "   " << "Average price is " << total_cost / order_volume << endl;
}