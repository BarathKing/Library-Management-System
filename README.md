# Library Management System in C++

## Overview
The Library Management System is a console-based application developed using C++ that manages books, members, and borrowing operations efficiently using Object-Oriented Programming and File Handling concepts.

The system allows librarians to manage books, register members, issue and return books, and maintain borrowing records with persistent data storage.

## Features

### Book Management
- Add New Books
- Display All Books
- Search Books by Title or Author
- Manage Available Copies

### Member Management
- Add New Members
- Display All Members
- Track Borrowed Books

### Borrow Operations
- Issue Books
- Return Books
- Due Date Management
- Fine Calculation for Late Returns
- View Borrow Records

### Additional Features
- Menu-Driven Interface
- Persistent Data Storage
- Validation and Error Handling
- File Handling using Binary Files

## Technologies Used
- C++
- Object-Oriented Programming (OOP)
- File Handling
- Binary File Storage

## OOP Concepts Implemented
- Classes and Objects
- Encapsulation
- Data Abstraction
- Member Functions
- Object Management

## File Storage
The system stores data using:
- `books.dat` → Stores book records
- `members.dat` → Stores member records
- `borrow.dat` → Stores borrowing records

## Borrowing Rules
- A member can borrow a maximum of 3 books
- Books are issued for 14 days
- Late fine: Rs.2 per day after due date
