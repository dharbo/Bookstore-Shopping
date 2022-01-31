#ifndef _book_database_hpp_
#define _book_database_hpp_

#include <string>
#include <vector>

#include "book.hpp"

// BookDatabase is an in-memory datastore, mapping ISBN numbers to Books.
class BookDatabase {
 public:
  // Returns a reference to the one and only instance of the database.
  static BookDatabase& instance();

  // Returns a pointer to the book in the database with the given ISBN.
  //
  // If no book exists in the database with the given ISBN, returns nullptr.
  Book* find(const std::string& isbn);

  // Returns the number of books in the database.
  std::size_t size() const;                                                   

 private:
  // Constructs a BookDatabase using the data read from `filename`.
  //
  // The constructor is private to prevent more than one instance from
  // existing.
  BookDatabase(const std::string& filename);

  // Intentionally prohibit copies.
  BookDatabase(const BookDatabase& other) = delete;

  // Intentionally prohibit copy assignments.
  BookDatabase& operator=(const BookDatabase& rhs) = delete;

  // The database that holds books.
  std::vector<Book> data_base_ = std::vector<Book>(); 

  // Recursive helper function to find a book by its isbn.
  Book* find(const std::string& isbn, int index);
};

#endif