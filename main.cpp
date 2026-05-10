/*
 * ============================================================
 *         LIBRARY MANAGEMENT SYSTEM
 *         Language  : C++
 *         Concepts  : OOP (Classes, Encapsulation, File Handling)
 *         Storage   : books.dat | members.dat | borrow.dat
 *         Features  : Add/Display/Search Books, Add Members,
 *                     Issue Book, Return Book, Fine Calculation
 * ============================================================
 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
//  UTILITY HELPERS
// ─────────────────────────────────────────────
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\n  Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printLine(char ch = '-', int width = 64) {
    cout << "  " << string(width, ch) << "\n";
}

void printHeader(const string& title) {
    clearScreen();
    cout << "\n";
    printLine('=');
    cout << setw(38 + title.size() / 2) << title << "\n";
    printLine('=');
    cout << "\n";
}

string getCurrentDate() {
    time_t now = time(0);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return string(buf);
}

// Returns today as days-since-epoch for fine calculation
long long toDays(const string& dateStr) {
    struct tm t = {};
    sscanf(dateStr.c_str(), "%d-%d-%d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900; t.tm_mon -= 1;
    return (long long)(mktime(&t) / 86400);
}

// ─────────────────────────────────────────────
//  CLASS: Book
// ─────────────────────────────────────────────
class Book {
    int  bookId;
    char title[60];
    char author[40];
    char genre[25];
    int  totalCopies;
    int  availableCopies;

public:
    Book() {}
    Book(int id, const string& t, const string& a,
         const string& g, int copies)
        : bookId(id), totalCopies(copies), availableCopies(copies) {
        t.copy(title,  59); title[min((int)t.size(),59)]  = '\0';
        a.copy(author, 39); author[min((int)a.size(),39)] = '\0';
        g.copy(genre,  24); genre[min((int)g.size(),24)]  = '\0';
    }

    // Getters
    int    getId()        const { return bookId; }
    string getTitle()     const { return string(title); }
    string getAuthor()    const { return string(author); }
    string getGenre()     const { return string(genre); }
    int    getTotal()     const { return totalCopies; }
    int    getAvailable() const { return availableCopies; }
    bool   isAvailable()  const { return availableCopies > 0; }

    // Issue / Return
    bool issueBook()  { if (availableCopies <= 0) return false; availableCopies--; return true; }
    void returnBook() { if (availableCopies < totalCopies) availableCopies++; }

    void display() const {
        cout << "  " << left
             << setw(6)  << bookId
             << setw(30) << title
             << setw(22) << author
             << setw(12) << genre
             << setw(7)  << totalCopies
             << availableCopies << "\n";
    }

    void displayFull() const {
        printLine();
        cout << "  Book ID        : " << bookId          << "\n"
             << "  Title          : " << title           << "\n"
             << "  Author         : " << author          << "\n"
             << "  Genre          : " << genre           << "\n"
             << "  Total Copies   : " << totalCopies     << "\n"
             << "  Available      : " << availableCopies << "\n";
        printLine();
    }

    // File I/O
    static void saveAll(const vector<Book>& list) {
        ofstream f("books.dat", ios::binary | ios::trunc);
        for (const auto& b : list)
            f.write(reinterpret_cast<const char*>(&b), sizeof(b));
    }

    static vector<Book> loadAll() {
        vector<Book> list;
        ifstream f("books.dat", ios::binary);
        Book b;
        while (f.read(reinterpret_cast<char*>(&b), sizeof(b)))
            list.push_back(b);
        return list;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Member
// ─────────────────────────────────────────────
class Member {
    int  memberId;
    char name[50];
    char email[40];
    char phone[15];
    int  booksBorrowed;   // current count

public:
    Member() {}
    Member(int id, const string& n, const string& e, const string& p)
        : memberId(id), booksBorrowed(0) {
        n.copy(name,  49); name[min((int)n.size(),49)]  = '\0';
        e.copy(email, 39); email[min((int)e.size(),39)] = '\0';
        p.copy(phone, 14); phone[min((int)p.size(),14)] = '\0';
    }

    int    getId()            const { return memberId; }
    string getName()          const { return string(name); }
    string getEmail()         const { return string(email); }
    string getPhone()         const { return string(phone); }
    int    getBooksBorrowed() const { return booksBorrowed; }

    void incrementBorrowed() { booksBorrowed++; }
    void decrementBorrowed() { if (booksBorrowed > 0) booksBorrowed--; }

    bool canBorrow() const { return booksBorrowed < 3; }  // max 3 books

    void display() const {
        cout << "  " << left
             << setw(8)  << memberId
             << setw(24) << name
             << setw(28) << email
             << setw(13) << phone
             << booksBorrowed << "\n";
    }

    static void saveAll(const vector<Member>& list) {
        ofstream f("members.dat", ios::binary | ios::trunc);
        for (const auto& m : list)
            f.write(reinterpret_cast<const char*>(&m), sizeof(m));
    }

    static vector<Member> loadAll() {
        vector<Member> list;
        ifstream f("members.dat", ios::binary);
        Member m;
        while (f.read(reinterpret_cast<char*>(&m), sizeof(m)))
            list.push_back(m);
        return list;
    }
};

// ─────────────────────────────────────────────
//  CLASS: BorrowRecord
// ─────────────────────────────────────────────
class BorrowRecord {
public:
    int  recordId;
    int  memberId;
    int  bookId;
    char issueDate[11];
    char dueDate[11];
    char returnDate[11];   // "----------" if not returned
    bool returned;

    BorrowRecord() {}
    BorrowRecord(int rid, int mid, int bid, const string& issue)
        : recordId(rid), memberId(mid), bookId(bid), returned(false) {
        issue.copy(issueDate, 10); issueDate[10] = '\0';

        // Due date = 14 days later
        long long d = toDays(issue) + 14;
        time_t t    = (time_t)(d * 86400);
        struct tm* tm_info = localtime(&t);
        strftime(dueDate, 11, "%Y-%m-%d", tm_info);

        string nd = "----------";
        nd.copy(returnDate, 10); returnDate[10] = '\0';
    }

    double calcFine() const {
        if (returned) return 0.0;
        long long today  = toDays(getCurrentDate());
        long long due    = toDays(string(dueDate));
        long long overdue = today - due;
        return (overdue > 0) ? overdue * 2.0 : 0.0;   // Rs.2 per day
    }

    void markReturned() {
        returned = true;
        string today = getCurrentDate();
        today.copy(returnDate, 10); returnDate[10] = '\0';
    }

    static void saveAll(const vector<BorrowRecord>& list) {
        ofstream f("borrow.dat", ios::binary | ios::trunc);
        for (const auto& r : list)
            f.write(reinterpret_cast<const char*>(&r), sizeof(r));
    }

    static vector<BorrowRecord> loadAll() {
        vector<BorrowRecord> list;
        ifstream f("borrow.dat", ios::binary);
        BorrowRecord r;
        while (f.read(reinterpret_cast<char*>(&r), sizeof(r)))
            list.push_back(r);
        return list;
    }

    static int nextId() {
        auto list = loadAll();
        return list.empty() ? 1 : list.back().recordId + 1;
    }
};

// ─────────────────────────────────────────────
//  CLASS: Library  (central controller)
// ─────────────────────────────────────────────
class Library {

    int nextBookId() {
        auto list = Book::loadAll();
        return list.empty() ? 101 : list.back().getId() + 1;
    }

    int nextMemberId() {
        auto list = Member::loadAll();
        return list.empty() ? 1001 : list.back().getId() + 1;
    }

public:
    // ══════════════════════════════
    //  BOOK OPERATIONS
    // ══════════════════════════════

    void addBook() {
        printHeader("  ADD NEW BOOK");

        string title, author, genre;
        int copies;

        cout << "  Enter Book Title   : "; cin.ignore(); getline(cin, title);
        cout << "  Enter Author Name  : "; getline(cin, author);
        cout << "  Enter Genre        : "; getline(cin, genre);

        while (true) {
            cout << "  Number of Copies  : ";
            if (cin >> copies && copies > 0) break;
            cout << "  [!] Must be at least 1.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        int id = nextBookId();
        Book b(id, title, author, genre, copies);
        auto list = Book::loadAll();
        list.push_back(b);
        Book::saveAll(list);

        cout << "\n";
        printLine('*');
        cout << "  [✓] Book added! Book ID : " << id << "\n";
        printLine('*');
        pauseScreen();
    }

    void displayAllBooks() {
        printHeader("  ALL BOOKS");

        auto list = Book::loadAll();
        if (list.empty()) { cout << "  No books found.\n"; pauseScreen(); return; }

        printLine();
        cout << "  " << left
             << setw(6)  << "ID"
             << setw(30) << "Title"
             << setw(22) << "Author"
             << setw(12) << "Genre"
             << setw(7)  << "Total"
             << "Avail\n";
        printLine();
        for (const auto& b : list) b.display();
        printLine();
        cout << "  Total Books : " << list.size() << "\n";
        pauseScreen();
    }

    // ── Search by Title or Author ──
    void searchBook() {
        printHeader("  SEARCH BOOKS");

        cout << "  Search by:\n"
             << "   1. Title\n"
             << "   2. Author\n\n"
             << "  Enter choice : ";
        int ch; cin >> ch; cin.ignore();

        string keyword;
        cout << "  Enter keyword : "; getline(cin, keyword);

        // Case-insensitive match
        string kLow = keyword;
        transform(kLow.begin(), kLow.end(), kLow.begin(), ::tolower);

        auto list = Book::loadAll();
        vector<Book> results;

        for (const auto& b : list) {
            string field = (ch == 1) ? b.getTitle() : b.getAuthor();
            transform(field.begin(), field.end(), field.begin(), ::tolower);
            if (field.find(kLow) != string::npos)
                results.push_back(b);
        }

        if (results.empty()) {
            cout << "\n  [!] No books found matching \"" << keyword << "\".\n";
        } else {
            cout << "\n  Found " << results.size() << " result(s):\n\n";
            printLine();
            cout << "  " << left
                 << setw(6)  << "ID"
                 << setw(30) << "Title"
                 << setw(22) << "Author"
                 << setw(12) << "Genre"
                 << setw(7)  << "Total"
                 << "Avail\n";
            printLine();
            for (const auto& b : results) b.display();
            printLine();
        }
        pauseScreen();
    }

    // ══════════════════════════════
    //  MEMBER OPERATIONS
    // ══════════════════════════════

    void addMember() {
        printHeader("  ADD NEW MEMBER");

        cin.ignore();
        string name, email, phone;
        cout << "  Enter Full Name    : "; getline(cin, name);
        cout << "  Enter Email        : "; getline(cin, email);
        cout << "  Enter Phone        : "; getline(cin, phone);

        int id = nextMemberId();
        Member m(id, name, email, phone);
        auto list = Member::loadAll();
        list.push_back(m);
        Member::saveAll(list);

        cout << "\n";
        printLine('*');
        cout << "  [✓] Member added! Member ID : " << id << "\n";
        printLine('*');
        pauseScreen();
    }

    void displayAllMembers() {
        printHeader("  ALL MEMBERS");

        auto list = Member::loadAll();
        if (list.empty()) { cout << "  No members found.\n"; pauseScreen(); return; }

        printLine();
        cout << "  " << left
             << setw(8)  << "ID"
             << setw(24) << "Name"
             << setw(28) << "Email"
             << setw(13) << "Phone"
             << "Borrowed\n";
        printLine();
        for (const auto& m : list) m.display();
        printLine();
        cout << "  Total Members : " << list.size() << "\n";
        pauseScreen();
    }

    // ══════════════════════════════
    //  ISSUE BOOK
    // ══════════════════════════════

    void issueBook() {
        printHeader("  ISSUE BOOK");

        int memberId, bookId;
        cout << "  Enter Member ID : "; cin >> memberId;
        cout << "  Enter Book ID   : "; cin >> bookId;

        auto members = Member::loadAll();
        auto books   = Book::loadAll();
        auto borrows = BorrowRecord::loadAll();

        // Find member
        auto mit = find_if(members.begin(), members.end(),
                            [&](const Member& m){ return m.getId() == memberId; });
        if (mit == members.end()) {
            cout << "\n  [!] Member ID " << memberId << " not found.\n";
            pauseScreen(); return;
        }
        if (!mit->canBorrow()) {
            cout << "\n  [!] Member has reached the borrowing limit (3 books).\n";
            pauseScreen(); return;
        }

        // Find book
        auto bit = find_if(books.begin(), books.end(),
                            [&](const Book& b){ return b.getId() == bookId; });
        if (bit == books.end()) {
            cout << "\n  [!] Book ID " << bookId << " not found.\n";
            pauseScreen(); return;
        }
        if (!bit->isAvailable()) {
            cout << "\n  [!] No copies of \"" << bit->getTitle() << "\" are available right now.\n";
            pauseScreen(); return;
        }

        // Check if member already borrowed same book
        for (const auto& r : borrows) {
            if (!r.returned && r.memberId == memberId && r.bookId == bookId) {
                cout << "\n  [!] Member already has this book issued.\n";
                pauseScreen(); return;
            }
        }

        // Issue
        bit->issueBook();
        mit->incrementBorrowed();

        BorrowRecord rec(BorrowRecord::nextId(), memberId, bookId, getCurrentDate());
        borrows.push_back(rec);

        Book::saveAll(books);
        Member::saveAll(members);
        BorrowRecord::saveAll(borrows);

        cout << "\n";
        printLine('*');
        cout << "  [✓] Book Issued Successfully!\n\n"
             << "  Member  : " << mit->getName()     << "\n"
             << "  Book    : " << bit->getTitle()     << "\n"
             << "  Issued  : " << rec.issueDate       << "\n"
             << "  Due By  : " << rec.dueDate         << "\n"
             << "  Fine    : Rs.2 per day after due date\n";
        printLine('*');
        pauseScreen();
    }

    // ══════════════════════════════
    //  RETURN BOOK
    // ══════════════════════════════

    void returnBook() {
        printHeader("  RETURN BOOK");

        int memberId, bookId;
        cout << "  Enter Member ID : "; cin >> memberId;
        cout << "  Enter Book ID   : "; cin >> bookId;

        auto members = Member::loadAll();
        auto books   = Book::loadAll();
        auto borrows = BorrowRecord::loadAll();

        // Find active borrow record
        auto rit = find_if(borrows.begin(), borrows.end(), [&](BorrowRecord& r) {
            return !r.returned && r.memberId == memberId && r.bookId == bookId;
        });

        if (rit == borrows.end()) {
            cout << "\n  [!] No active borrow record found for this member/book.\n";
            pauseScreen(); return;
        }

        double fine = rit->calcFine();
        rit->markReturned();

        // Update book and member
        for (auto& b : books)
            if (b.getId() == bookId) { b.returnBook(); break; }
        for (auto& m : members)
            if (m.getId() == memberId) { m.decrementBorrowed(); break; }

        Book::saveAll(books);
        Member::saveAll(members);
        BorrowRecord::saveAll(borrows);

        cout << "\n";
        printLine('*');
        cout << "  [✓] Book Returned Successfully!\n\n"
             << "  Issue Date  : " << rit->issueDate  << "\n"
             << "  Due Date    : " << rit->dueDate    << "\n"
             << "  Return Date : " << rit->returnDate << "\n";
        if (fine > 0)
            cout << "  [!] Late Fine : Rs. " << fixed << setprecision(2) << fine << "  (Please pay at the counter)\n";
        else
            cout << "  Fine        : No fine. Returned on time!\n";
        printLine('*');
        pauseScreen();
    }

    // ══════════════════════════════
    //  VIEW ALL BORROW RECORDS
    // ══════════════════════════════

    void viewBorrowRecords() {
        printHeader("  BORROW RECORDS");

        auto borrows = BorrowRecord::loadAll();
        auto books   = Book::loadAll();
        auto members = Member::loadAll();

        if (borrows.empty()) { cout << "  No borrow records found.\n"; pauseScreen(); return; }

        auto findBook   = [&](int id) -> string {
            for (auto& b : books) if (b.getId() == id) return b.getTitle(); return "Unknown";
        };
        auto findMember = [&](int id) -> string {
            for (auto& m : members) if (m.getId() == id) return m.getName(); return "Unknown";
        };

        printLine();
        cout << "  " << left
             << setw(5)  << "RID"
             << setw(20) << "Member"
             << setw(26) << "Book"
             << setw(12) << "Issued"
             << setw(12) << "Due"
             << setw(12) << "Returned"
             << "Fine\n";
        printLine();

        for (const auto& r : borrows) {
            double fine = r.calcFine();
            cout << "  " << left
                 << setw(5)  << r.recordId
                 << setw(20) << findMember(r.memberId).substr(0,18)
                 << setw(26) << findBook(r.bookId).substr(0,24)
                 << setw(12) << r.issueDate
                 << setw(12) << r.dueDate
                 << setw(12) << (r.returned ? r.returnDate : "Pending")
                 << (fine > 0 ? "Rs." + to_string((int)fine) : "None") << "\n";
        }
        printLine();
        cout << "  Total Records : " << borrows.size() << "\n";
        pauseScreen();
    }
};

// ─────────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────────
void showMenu() {
    printHeader("  LIBRARY MANAGEMENT SYSTEM");
    cout << "  ── BOOK OPERATIONS ──\n"
         << "   1.  Add Book\n"
         << "   2.  Display All Books\n"
         << "   3.  Search Book (Title / Author)\n\n"
         << "  ── MEMBER OPERATIONS ──\n"
         << "   4.  Add Member\n"
         << "   5.  Display All Members\n\n"
         << "  ── BORROW OPERATIONS ──\n"
         << "   6.  Issue Book\n"
         << "   7.  Return Book\n"
         << "   8.  View All Borrow Records\n\n"
         << "   9.  Exit\n\n";
    printLine();
    cout << "  Enter your choice (1-9) : ";
}

// ─────────────────────────────────────────────
//  ENTRY POINT
// ─────────────────────────────────────────────
int main() {
    Library lib;
    int choice;

    while (true) {
        showMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: lib.addBook();            break;
            case 2: lib.displayAllBooks();    break;
            case 3: lib.searchBook();         break;
            case 4: lib.addMember();          break;
            case 5: lib.displayAllMembers();  break;
            case 6: lib.issueBook();          break;
            case 7: lib.returnBook();         break;
            case 8: lib.viewBorrowRecords();  break;
            case 9:
                clearScreen();
                printLine('=');
                cout << "  Thank you for using Library Management System!\n";
                printLine('=');
                return 0;
            default:
                cout << "\n  [!] Invalid choice. Enter a number between 1 and 9.\n";
                pauseScreen();
        }
    }
}
