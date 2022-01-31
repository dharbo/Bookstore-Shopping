#include "book_database.hpp"

#include <fstream>
#include <string>

#include "book.hpp"

const std::string kDatabaseFilename = "database.txt";

// Return a reference to the one and only instance of the database
BookDatabase& BookDatabase::instance() {
  static BookDatabase theInstance(kDatabaseFilename);
  return theInstance;
}

BookDatabase::BookDatabase(const std::string& filename) {
  std::ifstream fin(filename, std::ios::binary);

  // The file contains Books separated by whitespace. A Book has 4 pieces of
  // data delimited with a comma. This exactly matches the previous assignment
  // as to how Books are read.
  //
  //       Field            Type            Notes
  //  1.   Book's ISBN      String          Unique identifier (primary key), always enclosed in double quotes
  //  2.   Book's Title     String          May contain spaces, always enclosed in double quotes
  //  3.   Book's Author    String          May contain spaces, always enclosed in double quotes
  //  4.   Book's Price     Floating Point  In dollars
  //
  //  Example:
  //    "0001062417",  "Early aircraft",                 "Maurice F. Allward", 65.65
  //    "0000255406",  "Shadow maker \"1st edition)\"",  "Rosemary Sullivan",   8.08
  //    "0000385264",  "Der Karawanenkardinal",          "Heinz Gstrein",      35.18
  //
  // NOTE: Double quotes within the string are escaped with the backslash
  // character.
  //
  
  Book book; //Create a book.
  while (fin >> book) { // Reads the books in the file.
    data_base_.push_back(book); // Push the books into the database.
  }

  // NOTE: The file is intentionally not explicitly closed. The file is closed
  // when fin goes out of scope - for whatever reason. More precisely, the
  // object `fin` is destroyed when it goes out of scope and the file is closed
  // in the destructor. See RAII.
}

// Wrapper find function
Book* BookDatabase::find(const std::string &isbn) {
  int index = 0; // Initialize index to 0.
  return find(isbn, index); // Call the find helper function.
}

// Helper find function
Book* BookDatabase::find(const std::string &isbn, int index) { 
  if (index == data_base_.size()) { // Base case.
    return nullptr; // The book is not in the database.
  } else if (data_base_[index].isbn() == isbn) { // The book is found.
    return &(data_base_[index]); // Returns a pointer to the book.
  }
  ++index; // Increment index, so we can compare the next book in the database.
  return find(isbn, index); // Call the same function with the next index.
}

// Returns the size of the database.
std::size_t BookDatabase::size() const {
  return data_base_.size();
}