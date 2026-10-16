// ============================================================
//  Online Bookstore - Admin System (C++ Console Project)
//  Features: Admin Login, Manage Books, Manage Customers,
//            Orders, Reports, File Storage (data survives restart)
//  Compile:  g++ -std=c++17 main.cpp -o bookstore
//  Run:      ./bookstore        (Windows: bookstore.exe)
//  Login:    username = admin   password = admin123
// ============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
using namespace std;

// ---------- Data structures ----------
struct Book {
    int id;
    string title, author, category;
    double price;
    int qty;
};
struct Customer {
    int id;
    string name, email;
};
struct Order {
    int id, customerId, bookId, qty;
    double total;
};

// ---------- Global data ----------
vector<Book> books;
vector<Customer> customers;
vector<Order> orders;

const string ADMIN_USER = "admin";
const string ADMIN_PASS = "admin123";

const string BOOKS_FILE = "books.txt";
const string CUSTOMERS_FILE = "customers.txt";
const string ORDERS_FILE = "orders.txt";

// ---------- Input helpers ----------
string readStr(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        replace(s.begin(), s.end(), '|', '/');   // '|' is our file separator
        if (!s.empty()) return s;
        cout << "  Input cannot be empty.\n";
    }
}

int readInt(const string &prompt) {
    while (true) {
        cout << prompt;
        string s;
        if (!getline(cin, s)) exit(0);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "  Please enter a valid whole number.\n";
    }
}

double readDouble(const string &prompt) {
    while (true) {
        cout << prompt;
        string s;
        if (!getline(cin, s)) exit(0);
        try {
            size_t pos;
            double v = stod(s, &pos);
            if (pos == s.size() && v >= 0) return v;
        } catch (...) {}
        cout << "  Please enter a valid positive number.\n";
    }
}

void pause() {
    cout << "\nPress Enter to continue...";
    string s;
    getline(cin, s);
}

vector<string> split(const string &line, char delim) {
    vector<string> parts;
    string item;
    stringstream ss(line);
    while (getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

// ---------- File handling ----------
void loadData() {
    string line;
    ifstream bf(BOOKS_FILE);
    while (getline(bf, line)) {
        auto p = split(line, '|');
        if (p.size() == 6)
            books.push_back({stoi(p[0]), p[1], p[2], p[3], stod(p[4]), stoi(p[5])});
    }
    ifstream cf(CUSTOMERS_FILE);
    while (getline(cf, line)) {
        auto p = split(line, '|');
        if (p.size() == 3) customers.push_back({stoi(p[0]), p[1], p[2]});
    }
    ifstream of(ORDERS_FILE);
    while (getline(of, line)) {
        auto p = split(line, '|');
        if (p.size() == 5)
            orders.push_back({stoi(p[0]), stoi(p[1]), stoi(p[2]), stoi(p[3]), stod(p[4])});
    }
}

void saveBooks() {
    ofstream f(BOOKS_FILE);
    for (auto &b : books)
        f << b.id << '|' << b.title << '|' << b.author << '|' << b.category << '|'
          << b.price << '|' << b.qty << '\n';
}
void saveCustomers() {
    ofstream f(CUSTOMERS_FILE);
    for (auto &c : customers) f << c.id << '|' << c.name << '|' << c.email << '\n';
}
void saveOrders() {
    ofstream f(ORDERS_FILE);
    for (auto &o : orders)
        f << o.id << '|' << o.customerId << '|' << o.bookId << '|' << o.qty << '|' << o.total << '\n';
}

// ---------- Lookup helpers ----------
Book *findBook(int id) {
    for (auto &b : books) if (b.id == id) return &b;
    return nullptr;
}
Customer *findCustomer(int id) {
    for (auto &c : customers) if (c.id == id) return &c;
    return nullptr;
}
int nextBookId()     { int m = 0; for (auto &b : books) m = max(m, b.id); return m + 1; }
int nextCustomerId() { int m = 0; for (auto &c : customers) m = max(m, c.id); return m + 1; }
int nextOrderId()    { int m = 0; for (auto &o : orders) m = max(m, o.id); return m + 1; }

string lower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// ---------- Display helpers ----------
void printBookHeader() {
    cout << left << setw(5) << "ID" << setw(28) << "Title" << setw(20) << "Author"
         << setw(14) << "Category" << setw(10) << "Price" << "Qty\n";
    cout << string(85, '-') << "\n";
}
void printBook(const Book &b) {
    cout << left << setw(5) << b.id << setw(28) << b.title.substr(0, 26)
         << setw(20) << b.author.substr(0, 18) << setw(14) << b.category.substr(0, 12)
         << setw(10) << fixed << setprecision(2) << b.price << b.qty << "\n";
}

// ============================================================
//  BOOKS
// ============================================================
void addBook() {
    cout << "\n--- Add New Book ---\n";
    Book b;
    b.id = nextBookId();
    b.title = readStr("Title    : ");
    b.author = readStr("Author   : ");
    b.category = readStr("Category : ");
    b.price = readDouble("Price    : ");
    b.qty = readInt("Quantity : ");
    books.push_back(b);
    saveBooks();
    cout << "Book added successfully (ID " << b.id << ").\n";
}

void viewBooks() {
    cout << "\n--- All Books ---\n";
    if (books.empty()) { cout << "No books available.\n"; return; }
    printBookHeader();
    for (auto &b : books) printBook(b);
}

void searchBooks() {
    cout << "\n--- Search Books ---\n";
    string key = lower(readStr("Enter title / author / category: "));
    bool found = false;
    for (auto &b : books) {
        if (lower(b.title).find(key) != string::npos ||
            lower(b.author).find(key) != string::npos ||
            lower(b.category).find(key) != string::npos) {
            if (!found) printBookHeader();
            printBook(b);
            found = true;
        }
    }
    if (!found) cout << "No matching books found.\n";
}

void updateBook() {
    cout << "\n--- Update Book ---\n";
    Book *b = findBook(readInt("Enter book ID: "));
    if (!b) { cout << "Book not found.\n"; return; }
    cout << "Leave blank is not allowed; re-enter all details.\n";
    b->title = readStr("New Title    : ");
    b->author = readStr("New Author   : ");
    b->category = readStr("New Category : ");
    b->price = readDouble("New Price    : ");
    b->qty = readInt("New Quantity : ");
    saveBooks();
    cout << "Book updated.\n";
}

void deleteBook() {
    cout << "\n--- Delete Book ---\n";
    int id = readInt("Enter book ID: ");
    auto it = remove_if(books.begin(), books.end(), [id](const Book &b) { return b.id == id; });
    if (it == books.end()) { cout << "Book not found.\n"; return; }
    books.erase(it, books.end());
    saveBooks();
    cout << "Book deleted.\n";
}

void booksMenu() {
    while (true) {
        cout << "\n===== MANAGE BOOKS =====\n"
             << "1. Add Book\n2. View All Books\n3. Search Books\n"
             << "4. Update Book\n5. Delete Book\n0. Back\n";
        int c = readInt("Choice: ");
        switch (c) {
            case 1: addBook(); break;
            case 2: viewBooks(); break;
            case 3: searchBooks(); break;
            case 4: updateBook(); break;
            case 5: deleteBook(); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
        }
        pause();
    }
}

// ============================================================
//  CUSTOMERS
// ============================================================
void addCustomer() {
    cout << "\n--- Add Customer ---\n";
    Customer c;
    c.id = nextCustomerId();
    c.name = readStr("Name  : ");
    c.email = readStr("Email : ");
    customers.push_back(c);
    saveCustomers();
    cout << "Customer added (ID " << c.id << ").\n";
}

void viewCustomers() {
    cout << "\n--- All Customers ---\n";
    if (customers.empty()) { cout << "No customers registered.\n"; return; }
    cout << left << setw(6) << "ID" << setw(25) << "Name" << "Email\n" << string(60, '-') << "\n";
    for (auto &c : customers)
        cout << left << setw(6) << c.id << setw(25) << c.name << c.email << "\n";
}

void deleteCustomer() {
    cout << "\n--- Delete Customer ---\n";
    int id = readInt("Enter customer ID: ");
    auto it = remove_if(customers.begin(), customers.end(), [id](const Customer &c) { return c.id == id; });
    if (it == customers.end()) { cout << "Customer not found.\n"; return; }
    customers.erase(it, customers.end());
    saveCustomers();
    cout << "Customer deleted.\n";
}

void customersMenu() {
    while (true) {
        cout << "\n===== MANAGE CUSTOMERS =====\n"
             << "1. Add Customer\n2. View Customers\n3. Delete Customer\n0. Back\n";
        int c = readInt("Choice: ");
        switch (c) {
            case 1: addCustomer(); break;
            case 2: viewCustomers(); break;
            case 3: deleteCustomer(); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
        }
        pause();
    }
}

// ============================================================
//  ORDERS
// ============================================================
void placeOrder() {
    cout << "\n--- Place Order ---\n";
    if (books.empty() || customers.empty()) {
        cout << "You need at least one book and one customer first.\n";
        return;
    }
    Customer *c = findCustomer(readInt("Customer ID: "));
    if (!c) { cout << "Customer not found.\n"; return; }
    Book *b = findBook(readInt("Book ID    : "));
    if (!b) { cout << "Book not found.\n"; return; }
    cout << "Selected: " << b->title << " (Stock: " << b->qty << ", Price: " << b->price << ")\n";
    int q = readInt("Quantity   : ");
    if (q <= 0) { cout << "Quantity must be at least 1.\n"; return; }
    if (q > b->qty) { cout << "Not enough stock.\n"; return; }

    Order o{nextOrderId(), c->id, b->id, q, b->price * q};
    b->qty -= q;
    orders.push_back(o);
    saveBooks();
    saveOrders();
    cout << "Order #" << o.id << " placed. Total = " << fixed << setprecision(2) << o.total << "\n";
}

void viewOrders() {
    cout << "\n--- All Orders ---\n";
    if (orders.empty()) { cout << "No orders yet.\n"; return; }
    cout << left << setw(8) << "Order" << setw(22) << "Customer" << setw(28) << "Book"
         << setw(6) << "Qty" << "Total\n" << string(75, '-') << "\n";
    for (auto &o : orders) {
        Customer *c = findCustomer(o.customerId);
        Book *b = findBook(o.bookId);
        cout << left << setw(8) << o.id
             << setw(22) << (c ? c->name.substr(0, 20) : "(deleted)")
             << setw(28) << (b ? b->title.substr(0, 26) : "(deleted)")
             << setw(6) << o.qty << fixed << setprecision(2) << o.total << "\n";
    }
}

void ordersMenu() {
    while (true) {
        cout << "\n===== ORDERS =====\n1. Place Order\n2. View Orders\n0. Back\n";
        int c = readInt("Choice: ");
        switch (c) {
            case 1: placeOrder(); break;
            case 2: viewOrders(); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
        }
        pause();
    }
}

// ============================================================
//  REPORTS
// ============================================================
void reports() {
    cout << "\n===== REPORTS =====\n";
    double stockValue = 0, sales = 0;
    int totalCopies = 0, lowStock = 0;
    for (auto &b : books) {
        stockValue += b.price * b.qty;
        totalCopies += b.qty;
        if (b.qty < 5) lowStock++;
    }
    for (auto &o : orders) sales += o.total;

    cout << fixed << setprecision(2)
         << "Total book titles   : " << books.size() << "\n"
         << "Total copies in stock: " << totalCopies << "\n"
         << "Stock value         : " << stockValue << "\n"
         << "Total customers     : " << customers.size() << "\n"
         << "Total orders        : " << orders.size() << "\n"
         << "Total sales         : " << sales << "\n"
         << "Low-stock titles (<5): " << lowStock << "\n";

    if (lowStock > 0) {
        cout << "\nLow stock books:\n";
        printBookHeader();
        for (auto &b : books) if (b.qty < 5) printBook(b);
    }
}

// ============================================================
//  LOGIN + DASHBOARD
// ============================================================
bool login() {
    cout << "=====================================\n"
         << "     ONLINE BOOKSTORE - ADMIN LOGIN\n"
         << "      Read * Learn * Grow\n"
         << "=====================================\n";
    for (int attempt = 1; attempt <= 3; attempt++) {
        string u, p;
        cout << "Username: ";
        if (!getline(cin, u)) exit(0);
        cout << "Password: ";
        if (!getline(cin, p)) exit(0);
        if (u == ADMIN_USER && p == ADMIN_PASS) {
            cout << "\nLogin successful. Welcome, Admin!\n";
            return true;
        }
        cout << "Invalid username or password. (" << 3 - attempt << " attempts left)\n\n";
    }
    return false;
}

void dashboard() {
    while (true) {
        cout << "\n=====================================\n"
             << "          ADMIN DASHBOARD\n"
             << "=====================================\n"
             << "1. Manage Books\n2. Manage Customers\n3. Orders\n4. Reports\n0. Logout\n";
        int c = readInt("Choice: ");
        switch (c) {
            case 1: booksMenu(); break;
            case 2: customersMenu(); break;
            case 3: ordersMenu(); break;
            case 4: reports(); pause(); break;
            case 0: cout << "Logged out. Goodbye!\n"; return;
            default: cout << "Invalid choice.\n";
        }
    }
}

int main() {
    loadData();
    if (!login()) {
        cout << "Too many failed attempts. Exiting.\n";
        return 1;
    }
    dashboard();
    return 0;
}
