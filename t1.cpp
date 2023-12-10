#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <fstream> // Include the header for ifstream and ofstream
#include <sstream> // Include the header for istringstream
#include <climits>
#include <queue>

struct ReservationNode
{
    int patronID;
    int priority;
    time_t timestamp;

    ReservationNode(int id, int prio)
        : patronID(id), priority(prio)
    {
        timestamp = time(0);
    }

    bool operator<(const ReservationNode &other) const
    {
        if (priority == other.priority)
        {
            return timestamp > other.timestamp;
        }
        return priority > other.priority;
    }
};

struct Book
{
    int bookID;
    std::string bookName;
    std::string authorName;
    bool availabilityStatus;
    int borrowedBy;
    // std::vector<ReservationNode> reservationHeap;
    std::priority_queue<ReservationNode> reservationHeap;

    Book(int id, const std::string &name, const std::string &author, bool available)
        : bookID(id), bookName(name), authorName(author), availabilityStatus(available), borrowedBy(-1)
    {
    }

    void addReservation(int patronID, int patronPriority)
    {
        reservationHeap.push(ReservationNode(patronID, patronPriority));
        // reservationHeap.emplace_back(patronID, patronPriority);
        // std::push_heap(reservationHeap.begin(), reservationHeap.end());
    }

    void removeReservation()
    {
        if (!reservationHeap.empty())
        {
            reservationHeap.pop();
            // std::pop_heap(reservationHeap.begin(), reservationHeap.end());
            // reservationHeap.pop_back();
        }
    }
};

enum Color
{
    RED,
    BLACK
};

// Represents a node in the Red-Black Tree.
struct RBNode
{
    Book *book;
    Color color;
    RBNode *left;
    RBNode *right;
    RBNode *parent;

    RBNode(Book *book) : book(book), color(RED), left(nullptr), right(nullptr), parent(nullptr) {} // Constructor to initialize an RBNode.
};

// Main class for the GatorLibrary system.
class GatorLibrary
{
private:
    RBNode *root;
    int colorFlipCount = 0;

    void leftRotate(RBNode *x)
    { // Performs a left rotation on the given node.

        RBNode *y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
        // colorFlipCount++;
    }

    void rightRotate(RBNode *y)
    { // Performs a right rotation on the given node.

        RBNode *x = y->left;
        y->left = x->right;
        if (x->right != nullptr)
            x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == nullptr)
            root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
        x->right = y;
        y->parent = x;
        // colorFlipCount++;
    }
    void insertFixup(RBNode *z)
    { // Fixes the Red-Black Tree after insertion
        while (z != root && z->parent->color == RED)
        {
            if (z->parent == z->parent->parent->left)
            {
                RBNode *y = z->parent->parent->right;
                if (y && y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                    colorFlipCount += 2;
                }
                else
                {
                    if (z == z->parent->right)
                    {
                        z = z->parent;
                        leftRotate(z);
                        // colorFlipCount += 1;
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                    colorFlipCount += 2;
                }
            }
            else
            {
                RBNode *y = z->parent->parent->left;
                if (y && y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                    colorFlipCount += 3;
                }
                else
                {
                    if (z == z->parent->left)
                    {
                        z = z->parent;
                        rightRotate(z);
                        // colorFlipCount += 1;
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                    colorFlipCount += 2;
                }
            }
        }
        root->color = BLACK;
    }

    void insertRB(Book *book)
    { // Inserts a new book into the Red-Black Tree.
        RBNode *z = new RBNode(book);
        RBNode *y = nullptr;
        RBNode *x = root;

        while (x != nullptr)
        {
            y = x;
            if (z->book->bookID < x->book->bookID)
                x = x->left;
            else
                x = x->right;
        }

        z->parent = y;
        if (y == nullptr)
            root = z;
        else if (z->book->bookID < y->book->bookID)
            y->left = z;
        else
            y->right = z;
        // colorFlipCount++;
        insertFixup(z);
    }

    void transplant(RBNode *u, RBNode *v)
    { // Transplants a node with another node.

        if (u->parent == nullptr)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v)
            v->parent = u->parent;
    }

    void deleteNode(RBNode *z)
    { // Deletes a node from the Red-Black Tree.

        RBNode *y = z;
        RBNode *x;
        Color yOriginalColor = y->color;

        if (z->left == nullptr)
        {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == nullptr)
        {
            x = z->left;
            transplant(z, z->left);
        }
        else
        {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z)
                x->parent = y;
            else
            {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        if (yOriginalColor == BLACK)
            deleteFixup(x);
        delete z;
    }

    void deleteFixup(RBNode *x)
    { // Fixes the Red-Black Tree after deletion.

        while (x != root && x && x->color == BLACK)
        {
            if (x == x->parent->left)
            {
                RBNode *w = x->parent->right;
                if (w->color == RED)
                { // Case 1: x's sibling w is red
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                    colorFlipCount += 2;
                }
                if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK))
                { // Case 2: Both of w's children are black
                    w->color = RED;
                    x = x->parent;
                    colorFlipCount += 1;
                }
                else
                {
                    if (!w->right || w->right->color == BLACK)
                    { // Case 3: w's right child is black
                        if (w->left)
                            w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                        colorFlipCount += 2;
                    }
                    w->color = x->parent->color; // Case 4: w's right child is red
                    x->parent->color = BLACK;
                    colorFlipCount += 1;
                    if (w->right)
                    {
                        w->right->color = BLACK;
                        colorFlipCount += 1;
                    }
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else
            {
                RBNode *w = x->parent->left;
                if (w->color == RED)
                {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                    colorFlipCount += 2;
                }
                if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK))
                {
                    w->color = RED;
                    x = x->parent;
                    colorFlipCount += 1;
                }
                else
                {
                    if (!w->left || w->left->color == BLACK)
                    {
                        if (w->right)
                        {
                            w->right->color = BLACK;
                            colorFlipCount += 1;
                        }
                        w->color = RED;
                        colorFlipCount += 1;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    colorFlipCount += 1;
                    if (w->left)
                    {

                        w->left->color = BLACK;
                        colorFlipCount += 1;
                    }
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        if (x)
        {

            x->color = BLACK;
            colorFlipCount += 1;
        }
    }

    RBNode *minimum(RBNode *node)
    { // Finds the node with the minimum key in the subtree.

        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    RBNode *maximum(RBNode *node)
    { // Finds the node with the maximum key in the subtree.

        while (node->right != nullptr)
            node = node->right;
        return node;
    }

    void inOrderTraversal(RBNode *node)
    { // Performs an in-order traversal of the Red-Black Tree.

        if (node != nullptr)
        {
            inOrderTraversal(node->left);
            std::cout << "BookID: " << node->book->bookID << " - Title: " << node->book->bookName << " - Author: " << node->book->authorName << " - Available: " << (node->book->availabilityStatus ? "Yes" : "No") << " - Borrowed by: " << node->book->borrowedBy << std::endl;
            inOrderTraversal(node->right);
        }
    }

    RBNode *findNode(RBNode *node, int bookID)
    { // Finds a node based on a given book ID.

        if (node == nullptr)
            return nullptr;
        if (bookID < node->book->bookID)
            return findNode(node->left, bookID);
        if (bookID > node->book->bookID)
            return findNode(node->right, bookID);
        return node;
    }

    void printBookInfo(const Book &book, std::ostream &output)
    { // Prints information about a book.

        output << "BookID: " << book.bookID << " - Title: " << book.bookName << " - Author: " << book.authorName << " - Available: " << (book.availabilityStatus ? "Yes" : "No") << " - Borrowed by: " << book.borrowedBy << std::endl;
    }

public:
    GatorLibrary() : root(nullptr), colorFlipCount(0) {} // Constructor for Gator Library.

    void InsertBook(int bookID, const std::string &bookName, const std::string &authorName, bool availability, int borrowedBy)
    { // Public methods for GatorLibrary operations...Includes methods like InsertBook, BorrowBook, ReturnBook, etc.

        Book *newBook = new Book(bookID, bookName, authorName, availability);
        if (availability)
        {
            insertRB(newBook);
            std::cout << "Book  inserted into Red-Black tree: ";
            return;
        }
        std::cout << "Book not inserted into Red-Black tree: ";
    }

    void BorrowBook(int patronID, int bookID, int patronPriority, std::ostream &output)
    {
        RBNode *node = findNode(root, bookID);

        if (node == nullptr)
            return;

        Book *book = node->book;

        if (book->availabilityStatus)
        {
            book->availabilityStatus = false;
            book->borrowedBy = patronID;
            output << "Book " << bookID << " Borrowed by Patron " << patronID << std::endl;
        }
        else
        {
            book->addReservation(patronID, patronPriority);
            output << "\nBook " << bookID << " Reserved by Patron " << patronID << std::endl;
        }
    }

    void ReturnBook(int patronID, int bookID, std::ostream &output)
    {
        RBNode *node = findNode(root, bookID);

        if (node == nullptr)
            return;

        Book *book = node->book;
        output << "Book " << bookID << " Returned by Patron " << patronID << std::endl
               << std::endl;
        // if (book->availabilityStatus)
        // {
        // Book is not borrowed. Handle this case.
        // }

        if (!book->reservationHeap.empty())
        {
            ReservationNode topReservation = book->reservationHeap.top();
            book->removeReservation();
            book->borrowedBy = topReservation.patronID;
            // BorrowBook(topReservation.patronID, bookID, topReservation.priority, output, false);
            output << "Book " << book->bookID << " Allotted to Patron " << topReservation.patronID << std::endl;
        }
        else
        {
            book->borrowedBy = -1;
            book->availabilityStatus = true;
        }
    }
    void PrintBook(int bookID, std::ostream &output)
    {
        RBNode *node = findNode(root, bookID);

        if (node)
        {
            Book *book = node->book;
            output << "\nBookID = " << book->bookID << std::endl;
            output << "Title = "
                   << "\"" << book->bookName << "\"" << std::endl;
            output << "Author = "
                   << "\"" << book->authorName << "\"" << std::endl;
            output << "Availability = \"" << (book->availabilityStatus ? "Yes\"" : "No\"") << std::endl;
            output << "BorrowedBy = " << (book->borrowedBy == -1 ? "None" : std::to_string(book->borrowedBy)) << std::endl;

            output << "Reservations = [";
            std::vector<int> reservedPatrons;
            auto temp = book->reservationHeap;
            while (!temp.empty())
            {
                reservedPatrons.push_back(temp.top().patronID);
                temp.pop();
            }
            for (size_t i = 0; i < book->reservationHeap.size(); ++i)
            {
                output << reservedPatrons[i];
                if (i < reservedPatrons.size() - 1)
                {
                    output << ", ";
                }
            }
            output << "]\n"
                   << std::endl;
        }
        else
        {
            output << "Book " << bookID << " not found in the Library" << std::endl;
        }
    }

    void PrintBooks(int bookID1, int bookID2, std::ostream &output)
    {
        inOrderTraversal(root, bookID1, bookID2, output);
    }

    void DeleteBook(int bookID, std::ostream &output)
    {
        RBNode *node = findNode(root, bookID);

        if (node == nullptr)
            return;

        Book *book = node->book;

        // Print the message before deleting the book
        output << "\nBook " << book->bookID << " is no longer available.";

        // Collect the patron IDs from the reservationHeap
        if (book->reservationHeap.empty())
        {
            output << std::endl;
            deleteNode(node);
            return;
        }
        if (book->reservationHeap.size() == 1)
        {
            output << " Reservation made by Patron ";
            output << book->reservationHeap.top().patronID << " has been cancelled!\n";
            deleteNode(node);
            return;
        }
        output << " Reservations made by Patrons ";
        std::vector<int> cancelledPatrons;
        auto temp = book->reservationHeap;
        while (!temp.empty())
        {
            cancelledPatrons.push_back(temp.top().patronID);
            temp.pop();
        }
        for (size_t i = 0; i < cancelledPatrons.size(); ++i)
        {
            output << cancelledPatrons[i];
            if (i < cancelledPatrons.size() - 1)
            {
                output << ", ";
            }
        }
        output << " have been cancelled!\n";

        deleteNode(node);
    }
    void inOrderTraversal(RBNode *node, int bookID1, int bookID2, std::ostream &output)
    {
        if (node == nullptr)
            return;

        if (node->book->bookID > bookID1)
            inOrderTraversal(node->left, bookID1, bookID2, output);

        PrintBook(node->book->bookID, output);

        if (node->book->bookID < bookID2)
            inOrderTraversal(node->right, bookID1, bookID2, output);
    }
    void FindClosestBook(int targetID, std::ostream &output)
    {
        if (!root)
        {
            output << "Library is empty." << std::endl;
            return;
        }

        RBNode *closestNode = nullptr;
        int closestDiff = INT_MAX;

        RBNode *node = findNode(root, targetID);
        if (node != nullptr)
        {
            PrintBook(targetID, output);
            return;
        }

        RBNode *current = root;
        while (current)
        {
            int diff = std::abs(current->book->bookID - targetID);
            if (diff < closestDiff)
            {
                closestDiff = diff;
                closestNode = current;
            }
            else if (diff == closestDiff)
            {
                // In case of a tie, add the book to a list of closest books
                if (!closestNode)
                {
                    closestNode = current;
                }
                else if (current->book->bookID < closestNode->book->bookID)
                {
                    closestNode = current;
                }
            }

            if (current->book->bookID == targetID)
            {
                closestNode = current;
                break;
            }

            if (current->book->bookID > targetID)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }

        inOrderTraversal(root, closestNode->book->bookID - 1, closestNode->book->bookID + 1, output);
    }

    void ColorFlipCount(std::ostream &output)
    {
        output << colorFlipCount;
    }
};

int main(int argc, char *argv[])
{ // Main logic for handling command-line arguments and running library operations. Includes file reading and writing, and executing library commands.
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " input_filename [operation1] [operation2] [...]" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string inputnametemp = inputFilename;
    std::string outputFilename = inputnametemp + "_output_file.txt";

    std::ifstream inputFileStream(inputFilename);
    if (!inputFileStream.is_open())
    {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return 1;
    }

    std::ofstream outputFile(outputFilename);

    GatorLibrary library;

    std::string line;
    while (std::getline(inputFileStream, line))
    {
        line.erase(std::remove(line.begin(), line.end(), '"'), line.end());
        line.rbegin()[0] = ',';
        std::stringstream iss(line);
        std::string command;
        std::getline(iss, command, '(');

        if (command == "InsertBook")
        {
            std::string bookID, title, author, availabilityStr;
            std::getline(iss, bookID, ',');
            iss.ignore();
            std::getline(iss, title, ',');
            iss.ignore();
            std::getline(iss, author, ',');
            iss.ignore();
            std::getline(iss, availabilityStr, ',');
            for (auto s : {bookID, title, author, availabilityStr})
            {
                std::cout << s << std::endl;
            }
            library.InsertBook(std::stoi(bookID), title, author, availabilityStr == "Yes", -1);
        }
        else if (command == "PrintBook")
        {
            std::string bookID;
            std::getline(iss, bookID, ',');
            library.PrintBook(std::stoi(bookID), outputFile);
        }
        else if (command == "PrintBooks")
        {
            std::string bookID1, bookID2;
            std::getline(iss, bookID1, ',');
            iss.ignore();
            std::getline(iss, bookID2, ',');
            library.PrintBooks(std::stoi(bookID1), std::stoi(bookID2), outputFile);
        }
        else if (command == "BorrowBook")
        {
            std::string patronID, bookID, patronPriority;
            std::getline(iss, patronID, ',');
            iss.ignore();
            std::getline(iss, bookID, ',');
            iss.ignore();
            std::getline(iss, patronPriority, ',');
            library.BorrowBook(std::stoi(patronID), std::stoi(bookID), std::stoi(patronPriority), outputFile);
        }
        else if (command == "ReturnBook")
        {
            std::string patronID, bookID;
            std::getline(iss, patronID, ',');
            iss.ignore();
            std::getline(iss, bookID, ',');
            library.ReturnBook(std::stoi(patronID), std::stoi(bookID), outputFile);
        }
        else if (command == "FindClosestBook")
        {
            std::string patronID;
            std::getline(iss, patronID, ',');
            library.FindClosestBook(std::stoi(patronID), outputFile);
        }
        else if (command == "DeleteBook")
        {
            std::string bookID;
            std::getline(iss, bookID, ',');
            library.DeleteBook(std::stoi(bookID), outputFile);
        }
        else if (command == "ColorFlipCount")
        {
            outputFile << "Color Flip Count: ";
            library.ColorFlipCount(outputFile);
            outputFile << std::endl;
        }
        else if (command == "Quit")
        {
            break;
        }
    }
    outputFile << "\nProgram Terminated!!" << std::endl;
    inputFileStream.close();
    outputFile.close();

    return 0;
}