#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
 
using namespace std;
using namespace boost::filesystem;

void GetIntFromFile(path filepath);
boost::mutex locker;
vector<int> allnumbers;

int main(int argc, char *argv[]){
    
	path p(argv[1]);
 
    directory_iterator end_itr;
	vector<string> filelist;
	vector<boost::thread *> ThreadsVector;
	
    // cycle through directory
    for (directory_iterator itr(p); itr != end_itr; ++itr){
        // If it's a file, add it to vector.
        if (is_regular_file(itr->path())) {
            filelist.push_back(itr->path().string());
		}
    }
	
	//create a vector of threads
	while (!filelist.empty()){
		ThreadsVector.push_back(new boost::thread(GetIntFromFile, filelist.back())); //create a new thread for 
		filelist.pop_back();
	}
	
	while(!ThreadsVector.empty()){
		ThreadsVector.back()->join();
		ThreadsVector.pop_back();
	}

	//count all int values from files
	int accum = 0;
	while(!allnumbers.empty()){
		accum += allnumbers.back();
		allnumbers.pop_back();
	}
	cout << endl << "summ: " << accum << endl;

return 0;
}

void GetIntFromFile(path filepath){
	std::ifstream myfile;
	myfile.open(filepath.string());
	string value;
	int number;
	myfile >> value;
	
	try { //if in file not int - go to catch and return
		number = boost::lexical_cast<int>(value); // string2int
	}
	catch(boost::bad_lexical_cast& error) {
		return;
	}
	myfile.close();

	locker.lock(); //depricate another threads write to vector and stdout
	allnumbers.push_back(number);
	cout << filepath.filename().string() << ": " << number << endl;
	locker.unlock();

	boost::this_thread::sleep_for(boost::chrono::seconds(1)); //sleep thread for 1 sec
}