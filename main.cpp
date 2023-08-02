#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <queue>
#include <random>
#include <ctime>

using namespace std;
using namespace chrono;

struct House {
    string address;
    string state;
    int bedrooms;
    int bathrooms;
    int price;
    int houseSize;
};

// Node structure
struct Node {
    House house;
    shared_ptr<Node> left;
    shared_ptr<Node> right;

    Node(const House& m) : house(m), left(nullptr), right(nullptr) {}
};

// BST class
class BST {
private:
    shared_ptr<Node> root;

    // Helper function for inserting a house into the BST
    void insert(shared_ptr<Node>& node, const House& house) {
        if (!node) {
            node = make_shared<Node>(house);
        }
        else if (house.price < node->house.price) {
            insert(node->left, house);
        }
        else {
            insert(node->right, house);
        }
    }

    // Helper function for filtering houses by maximum price and minimum size
    void in_order_traversal(shared_ptr<Node> node, const string &chosen_state, int max_price, BST &size_filtered_tree, int min_size) {
        if (!node) {
            return;
        }

        in_order_traversal(node->left, chosen_state, max_price, size_filtered_tree, min_size);

        // Check if the house belongs to the chosen state
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

// Helper function for inserting houses into a vector using in-order traversal
    void in_order_traversal_to_vector(shared_ptr<Node> node, vector<House> &houses) {
        if (!node) {
            return;
        }

        in_order_traversal_to_vector(node->left, houses);
        houses.push_back(node->house);
        in_order_traversal_to_vector(node->right, houses);
    }

    // Helper function for DFS traversal
    void dfs(shared_ptr<Node> node) {
        if (!node) {
            return;
        }

        dfs(node->left);
        dfs(node->right);
    }

    // Helper function for choosing a random house using DFS traversal
    House choose_random_house_dfs(shared_ptr<Node> node, int &count, int target_count) {
        if (!node) {
            return House{"", "", 0, 0, 0, 0};
        }
        House left_house = choose_random_house_dfs(node->left, count, target_count);
        if (!left_house.address.empty()) {
            return left_house;
        }
        if (count == target_count) {
            return node->house;
        }
        count++;
        return choose_random_house_dfs(node->right, count, target_count);
    }

    // Helper function for choosing a random house using BFS traversal
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
            if (current->left) {
                q.push(current->left);
            }
            if (current->right) {
                q.push(current->right);
            }
        }
        return House{"", "", 0, 0, 0, 0};
    }

public:
    BST() : root(nullptr) {}
    // Public function for inserting a house
    void insert(const House& house) {
        insert(root, house);
    }
    // Public function for filtering houses by maximum price and minimum size
    void filter_houses_by_size(const string &chosen_state,int max_price, BST &size_filtered_tree, int min_size) {
        in_order_traversal(root, chosen_state, max_price, size_filtered_tree, min_size);
    }
    // Public function for inserting houses into a vector using in-order traversal
    void filtered_houses_to_vector(vector<House> &houses) {
        in_order_traversal_to_vector(root, houses);
    }

    // DFS traversal
    void dfs_traversal() {
        dfs(root);
    }
    // BFS traversal
    void bfs_traversal() {
        if (!root) {
            return;
        }
        queue<shared_ptr<Node>> q;
        q.push(root);
        while (!q.empty()) {
            shared_ptr<Node> current = q.front();
            q.pop();
            if (current->left) {
                q.push(current->left);
            }
            if (current->right) {
                q.push(current->right);
            }
        }
    }
};

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
            // Ignore any lines that can't be processed
            continue;
        }
    }
}

// Function for reading housing data from a CSV file and storing it in a map
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
            // Split the CSV row into tokens
            while (getline(ss, token, ',')) {
                row.push_back(token);
            }
            // Extract housing information from the tokens
            string address = row[5];           // address of house
            string state = row[8];             // state of house
            int bedrooms = stoi(row[2]);       // bedrooms
            int bathrooms = stoi(row[3]);      // bathrooms
            int price = stoi(row[1]);          // price of the house in USD
            int houseSize = stoi(row[10]);     // size of house in square feet
            House house{address, state, bedrooms, bathrooms, price, houseSize};
            // Add the house to the map using its title as the key
            houses[address] = house;
        }
        catch (const exception &e) {
            // Ignore any lines that can't be processed
            continue;
        }
    }
}

// Function for printing all houses in a map to the console
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

// Function for selecting a random house from a map
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

int main() {
    unordered_map<string, vector<House>> state_to_houses;
    read_csv_file("realtordata.csv", state_to_houses);
    unordered_map<string, House> houses;
    read_csv_file_map("realtordata.csv", houses);
    cout << "Ready to Search for Your Dream Home?" << endl;
    cout << "What is your name?" << endl;
    string name = "";
    cin >> name;
    int choice;
    bool runAgain = true;
    while (runAgain) {
        cout << "Hi " << name << ", choose an option:" << endl;
        cout << "1. Find house using filters via DFS and BFS" << endl;
        cout << "2. Surprise me!" << endl;
        cout << "3. Print all houses utilizing unordered map" << endl;
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
            // Create BST and insert houses of chosen state
            BST house_bst;
            for (const auto &house: state_to_houses[state]) {
                house_bst.insert(house);
            }
            // Create BST for size filtered houses
            BST size_filtered_tree;
            // Filter houses by max price and min size
            house_bst.filter_houses_by_size(state, max_price, size_filtered_tree, min_size);
            // Store the filtered houses in a vector
            vector<House> final_houses;
            size_filtered_tree.filtered_houses_to_vector(final_houses);
            // Measure time for DFS and BFS traversals on house_bst
            auto start_dfs_1 = high_resolution_clock::now();
            house_bst.dfs_traversal();
            // auto stop_d;
            auto stop_dfs_1 = high_resolution_clock::now();
            auto duration_dfs_1 = duration_cast<microseconds>(stop_dfs_1 - start_dfs_1);

            auto start_bfs_1 = high_resolution_clock::now();
            house_bst.bfs_traversal();
            auto stop_bfs_1 = high_resolution_clock::now();
            auto duration_bfs_1 = duration_cast<microseconds>(stop_bfs_1 - start_bfs_1);

            // Measure time for DFS and BFS traversals on size_filtered_tree
            auto start_dfs_2 = high_resolution_clock::now();
            size_filtered_tree.dfs_traversal();
            auto stop_dfs_2 = high_resolution_clock::now();
            auto duration_dfs_2 = duration_cast<microseconds>(stop_dfs_2 - start_dfs_2);

            auto start_bfs_2 = high_resolution_clock::now();
            size_filtered_tree.bfs_traversal();
            auto stop_bfs_2 = high_resolution_clock::now();
            auto duration_bfs_2 = duration_cast<microseconds>(stop_bfs_2 - start_bfs_2);

            // Calculate total time for DFS and BFS traversals
            auto total_dfs_time = duration_dfs_1 + duration_dfs_2;
            auto total_bfs_time = duration_bfs_1 + duration_bfs_2;
            // Print the final_houses vector
            if (final_houses.empty()) {
                cout << "No houses found with the specified filters." << endl;
                cout << "Suggesting an alternative house by relaxing the filters..." << endl << endl;
                // Relax the filters
                int relaxed_max_price = max_price + 10000;
                int relaxed_min_size = min_size - 200;
                if (relaxed_min_size < 0.0) {
                    relaxed_min_size = 0.0;
                }
                // Filter houses with relaxed filters
                BST relaxed_size_filtered_tree;
                house_bst.filter_houses_by_size(state, relaxed_max_price, relaxed_size_filtered_tree,
                                                      relaxed_min_size);
                // Store the filtered houses in a vector
                vector<House> relaxed_final_houses;
                relaxed_size_filtered_tree.filtered_houses_to_vector(relaxed_final_houses);
                // Print the relaxed_final_houses vector
                for (const House &house: relaxed_final_houses) {
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
            cout << "Total DFS traversal time: " << total_dfs_time.count() << " microseconds" << endl;
            cout << "Total BFS traversal time: " << total_bfs_time.count() << " microseconds" << endl;
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
        int continueChoice;
        cout << endl <<"Would you like to start over or exit?" << endl;
        cout << "1. Start over" << endl;
        cout << "2. Exit" << endl;
        cin >> continueChoice;

        if (continueChoice == 2) {
            runAgain = false;
        }
    }
    return 0;
}