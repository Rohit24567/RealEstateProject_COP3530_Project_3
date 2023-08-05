#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <random>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

using namespace std;
using namespace chrono;

// struct for the House class including all the features of the house
struct House {
    string address;
    string state;
    int bedrooms;
    int bathrooms;
    int price;
    int houseSize;
};

// struct for the Node class and how the graph will be created
struct Node {
    House house;
    shared_ptr<Node> left;
    shared_ptr<Node> right;

    Node(const House& m) : house(m), left(nullptr), right(nullptr) {}
};

class BST {
private:
    shared_ptr<Node> root;
    // Insert function to decide where the node should be inserted in the BST
    void insert(shared_ptr<Node>& node, const House& house) {
        if (!node) {
            node = make_shared<Node>(house);
        }
        else if (house.price >= node->house.price) {
            insert(node->right, house);
        }
        else {
            insert(node->left, house);
        }
    }
    // Function to help filter the tree based on the users max_price and min_size inputs
    void in_order_traversal(shared_ptr<Node> node, const string &chosen_state, int max_price, BST &size_filtered_tree, int min_size) {
        if (!node) {
            return;
        }
        in_order_traversal(node->left, chosen_state, max_price, size_filtered_tree, min_size);

        if (node->house.state.find(chosen_state) != string::npos) {
            if (node->house.price <= max_price && node->house.houseSize >= min_size) {
                size_filtered_tree.insert(node->house);
            }
            else {
                return;
            }
        }
        in_order_traversal(node->right, chosen_state, max_price, size_filtered_tree, min_size);
    }
    // Function to create a vector with all the filtered houses
    void in_order_traversal_to_vector(shared_ptr<Node> node, vector<House> &houses) {
        if (!node) {
            return;
        }

        in_order_traversal_to_vector(node->left, houses);
        houses.push_back(node->house);
        in_order_traversal_to_vector(node->right, houses);
    }
    // Function for the surprise feature of the program using BFS
    House choose_random_house_bfs(int target_count) {
        if (!root) {
            return House{"", "", 0, 0, 0, 0};
        }
        int count = 0;
        queue<shared_ptr<Node>> q;
        q.push(root);
        while (!q.empty()) {
            shared_ptr<Node> current = q.front();
            q.pop();
            if (count == target_count) {
                return current->house;
            }
            count++;
            if (current->right) {
                q.push(current->right);
            }
            if (current->left) {
                q.push(current->left);
            }
        }
        return House{"", "", 0, 0, 0, 0};
    }
    // DFS traversal
    void dfs(shared_ptr<Node> node) {
        if (!node) {
            return;
        }

        dfs(node->left);
        dfs(node->right);
    }
    // Function for the surprise feature of the program using DFS
    House choose_random_house_dfs(shared_ptr<Node> node, int &count, int target_count) {
        if (!node) {
            return House{"", "", 0, 0, 0, 0};
        }
        House left_house = choose_random_house_dfs(node->left, count, target_count);
        if (count == target_count) {
            return node->house;
        }
        if (!left_house.address.empty()) {
            return left_house;
        }
        count++;
        return choose_random_house_dfs(node->right, count, target_count);
    }

public:
    BST() : root(nullptr) {}
    // Inserting a house into a BST
    void insert(const House& house) {
        insert(root, house);
    }
    // Public function to help filter the tree based on the users max_price and min_size inputs
    void filter_houses_by_size(const string &chosen_state,int max_price, BST &size_filtered_tree, int min_size) {
        in_order_traversal(root, chosen_state, max_price, size_filtered_tree, min_size);
    }
    // Public function to create a vector with all the filtered houses
    void filtered_houses_to_vector(vector<House> &houses) {
        in_order_traversal_to_vector(root, houses);
    }
    // Public BFS traversal
    void bfs_traversal() {
        if (!root) {
            return;
        }
        queue<shared_ptr<Node>> q;
        q.push(root);
        while (!q.empty()) {
            shared_ptr<Node> current = q.front();
            q.pop();
            if (current->right) {
                q.push(current->right);
            }
            if (current->left) {
                q.push(current->left);
            }
        }
    }
    // Public DFS traversal
    void dfs_traversal() {
        dfs(root);
    }
};

// Function used to read the CSV file and store the houses in a vector containing the house struct
void read_csv_file(const string &filename, unordered_map<string, vector<House>> &state_to_houses) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        try {
            stringstream ss(line);
            string token;
            vector<string> row;
            while (getline(ss, token, ',')) {
                row.push_back(token);
            }
            string address = row[5];           // address of house
            string state = row[8];             // state of house
            int bedrooms = stoi(row[2]);       // number of bedrooms
            int bathrooms = stoi(row[3]);      // number of bathrooms
            int price = stoi(row[1]);          // price of the house in USD
            int houseSize = stoi(row[10]);     // size of house in square feet
            House house{address, state, bedrooms, bathrooms, price, houseSize};
            state_to_houses[state].push_back(house);
        } catch (const exception &e) {
            continue;
        }
    }
}
// Function used to read the CSV file and store the houses in an unordered map
void read_csv_file_map(const string &filename, unordered_map<string, House> &houses) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    // Skip the header line
    getline(file, line);
    while (getline(file, line)) {
        try {
            stringstream ss(line);
            string token;
            vector<string> row;
            while (getline(ss, token, ',')) {
                row.push_back(token);
            }
            string address = row[5];           // address of house
            string state = row[8];             // state of house
            int bedrooms = stoi(row[2]);       // bedrooms
            int bathrooms = stoi(row[3]);      // bathrooms
            int price = stoi(row[1]);          // price of the house in USD
            int houseSize = stoi(row[10]);     // size of house in square feet
            House house{address, state, bedrooms, bathrooms, price, houseSize};
            houses[address] = house;
        }
        catch (const exception &e) {
            continue;
        }
    }
}
// Function used to select a random house from the map
House select_random_house_from_map(const unordered_map<string, House> &houses) {
    if (houses.empty()) {
        return House{"", "", 0, 0, 0, 0};
    }
    // Seed random number generator
    srand(time(0));
    int random_index = rand() % houses.size();

    int current_index = 0;
    for (const auto &house_pair : houses) {
        if (current_index == random_index) {
            return house_pair.second;
        }
        current_index++;
    }
    return House{"", "", 0, 0, 0, 0};
}
// Function used to print all the features of the houses
void printAllHouses(unordered_map<string, House> houses){
    for (const auto &house : houses) {
        cout << "Address: " << house.second.address << endl;
        cout << "State: " << house.second.state << endl;
        cout << "# of Bedrooms: " << house.second.bedrooms << endl;
        cout << "# of Bathrooms: " << house.second.bathrooms << endl;
        cout << "House Size (sq. ft): " << house.second.houseSize << endl;
        cout << endl;
    }
}

int main() {
    unordered_map<string, vector<House>> state_to_houses;
    read_csv_file("realtordata.txt", state_to_houses);
    unordered_map<string, House> houses;
    read_csv_file_map("realtordata.txt", houses);
    cout << "Ready to Search for Your Dream Home?" << endl;
    cout << "What is your name?" << endl;
    string name = "";
    cin >> name;
    int choice;
    bool runAgain = true;
    while (runAgain) {
        cout << "Hi " << name << ", what are you looking for today:" << endl;
        cout << "1. Find house using price and size" << endl;
        cout << "2. Find me a random house!" << endl;
        cout << "3. Print all possible houses in the unordered map" << endl;
        cin >> choice;
        if (choice == 1) {
            string state;
            int max_price;
            int min_size;
            // User input
            cout << "Which state would you like to search in? (enter with capital first letter)?" << endl;
            cin >> state;
            while (state != "Puerto Rico" && state != "Virgin Islands" && state != "Alabama" && state != "Alaska" && state != "Arizona" && state != "Arkansas" && state != "California" && state != "Colorado" && state != "Connecticut" && state != "Delaware" && state != "Florida" && state != "Georgia" && state != "Hawaii" && state != "Idaho" && state != "Illinois" && state != "Indiana" && state != "Iowa" && state != "Kansas" && state != "Kentucky" && state != "Louisiana" && state != "Maine" && state != "Maryland" && state != "Massachusetts" && state != "Michigan" && state != "Minnesota" && state != "Mississippi" && state != "Missouri" && state != "Montana" && state != "Nebraska" && state != "Nevada" && state != "New Hampshire" && state != "New Jersey" && state != "New Mexico" && state != "New York" && state != "North Carolina" && state != "North Dakota" && state != "Ohio" && state != "Oklahoma" && state != "Oregon" && state != "Pennsylvania" && state != "Rhode Island" && state != "South Carolina" && state != "South Dakota" && state != "Tennessee" && state != "Texas" && state != "Utah" && state != "Vermont" && state != "Virginia" && state != "Washington" && state != "West Virginia" && state != "Wisconsin" && state != "Wyoming") {
                cout << "Invalid state" << endl;
                cout << "Try again!" << endl;
                state = " ";
                cin >> state;
            }
            state += "\r";
            cout << "What is the maximum price you want the house to be? (in USD)?: " << endl;
            cin >> max_price;
            cout << "What is the minimum size (sq. ft) you want your dream home to be? : " << endl;
            cin >> min_size;
            BST house_bst;
            for (const auto &house: state_to_houses[state]) {
                house_bst.insert(house);
            }
            BST size_filtered_tree;
            // Filter houses by max price and min size
            house_bst.filter_houses_by_size(state, max_price, size_filtered_tree, min_size);
            vector<House> final_houses;
            size_filtered_tree.filtered_houses_to_vector(final_houses);

            auto start_dfs = high_resolution_clock::now();
            house_bst.dfs_traversal();
            auto stop_dfs = high_resolution_clock::now();
            auto duration_dfs = duration_cast<microseconds>(stop_dfs - start_dfs);

            auto start_dfs_next = high_resolution_clock::now();
            size_filtered_tree.dfs_traversal();
            auto stop_dfs_next = high_resolution_clock::now();
            auto duration_dfs_next = duration_cast<microseconds>(stop_dfs_next - start_dfs_next);

            auto start_bfs = high_resolution_clock::now();
            house_bst.bfs_traversal();
            auto stop_bfs = high_resolution_clock::now();
            auto duration_bfs = duration_cast<microseconds>(stop_bfs - start_bfs);

            auto start_bfs_next = high_resolution_clock::now();
            size_filtered_tree.bfs_traversal();
            auto stop_bfs_next = high_resolution_clock::now();
            auto duration_bfs_next = duration_cast<microseconds>(stop_bfs_next - start_bfs_next);

            auto dfs_time = duration_dfs + duration_dfs_next;
            auto bfs_time = duration_bfs + duration_bfs_next;
            // Print the final_houses vector
            if (final_houses.empty()) {
                cout << "No houses found with the specified filters." << endl;
                cout << "Suggesting an alternative house by fixing the filters..." << endl << endl;
                // Relax the filters
                int new_max_price = max_price + 10000;
                int new_min_size = min_size - 200;
                if (new_min_size < 0.0) {
                    new_min_size = 0.0;
                }
                BST new_size_filtered_tree;
                house_bst.filter_houses_by_size(state, new_max_price, new_size_filtered_tree,
                                                      new_min_size);

                vector<House> new_final_houses;
                new_size_filtered_tree.filtered_houses_to_vector(new_final_houses);

                for (const House &house: new_final_houses) {
                    cout << house.address << " (" << house.bedrooms << " bedrooms, " << house.bathrooms << " bathrooms, " << house.houseSize << " square feet)"
                         << endl;
                }
            }
            else {
                for (const House &house: final_houses) {
                    cout << house.address << " (" << house.bedrooms << " bedrooms, " << house.bathrooms << " bathrooms, " << house.houseSize << " square feet)"
                         << endl << endl;
                }
            }
            // Print time measurements
            cout << "DFS traversal time: " << dfs_time.count() << " microseconds" << endl;
            cout << "BFS traversal time: " << bfs_time.count() << " microseconds" << endl;
        }
        else if (choice == 2) {
            House random_house = select_random_house_from_map(houses);
            cout << "Address: " << random_house.address << endl;
            cout << "State: " << random_house.state << endl;
            cout << "# of Bedrooms: " << random_house.bedrooms << endl;
            cout << "# of Bathrooms: " << random_house.bathrooms << endl;
            cout << "House Size (sq. ft): " << random_house.houseSize << endl;
            cout << endl;
        }
        else if (choice == 3) {
            printAllHouses(houses);
        }
        int finalChoice;
        cout << endl <<"Would you like to restart or exit?" << endl;
        cout << "1. Restart" << endl;
        cout << "2. Exit" << endl;
        cin >> finalChoice;

        if (finalChoice == 2) {
            runAgain = false;
        }
    }
    return 0;
}