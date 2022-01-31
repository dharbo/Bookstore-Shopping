#include "checkout.hpp"

#include <iomanip>
#include <map>
#include <queue>
#include <stack>

#include "book.hpp"

namespace {

constexpr bool kOutputTrace = false;

// Call this function from within the carefully_move_books functions, just
// before kicking off the recursion and then just after each move.
void trace(
    std::stack<Book> const& sourceCart,
    std::stack<Book> const& destinationCart,
    std::stack<Book> const& spareCart,
    std::ostream& s = std::clog) {
  // Count and label the number of moves
  static std::size_t move_number = 0;

  // First time called will bind parameters to copies
  static std::map<std::stack<Book> const*, std::stack<Book>> book_carts = {
     {&sourceCart, {}},
     {&destinationCart, {}},
     {&spareCart, {}}
  };
  static std::map<std::stack<Book> const*, std::string> col_labels = {
      {&sourceCart, "Broken Cart"},
      {&destinationCart, "Working Cart"},
      {&spareCart, "Spare Cart"}
  };

  // Interrogating the stacks is a destructive process, so local copies of the
  // parameters are made to work with. The CarefullyMoveBooks algorithm will
  // swap the order of the arguments passed to this functions, but they will
  // always be the same objects - just in different orders.
  //
  // When outputting the stack contents, keep the original order so we humans
  // can trace the movements easier. A container (std::map) indexed by the
  // object's identity (address) is created so the canonical order remains the
  // same from one invocation to the next.
  book_carts[&sourceCart] = sourceCart;
  book_carts[&destinationCart] = destinationCart;
  book_carts[&spareCart] = spareCart;


  // Determine the size of the tallest stack.
  std::size_t tallest_stack_size = 0;
  for (auto& book_cart : book_carts) {
    if (book_cart.second.size() > tallest_stack_size) {
      tallest_stack_size = book_cart.second.size();
    }
  }

  if (kOutputTrace) {
    // Print the header
    s << "After " << std::setw( 3 ) << move_number++ << " moves:     " << std::left;      // print the move number
    for (const auto& col_label : col_labels) {
       s << std::setw( 23 ) << col_label.second;
    }
    s << std::right << "\n                     " << std::string( 23*3, '-' ) << '\n';     // underline the labels

    // Print the stack's contents.
    for (; tallest_stack_size > 0;  --tallest_stack_size) {
      s << std::string( 21, ' ' );

      for (auto& book_cart : book_carts) {
        if (book_cart.second.size() == tallest_stack_size) {
          std::string title = book_cart.second.top().title();
          if (title.size() > 20) {
            title[17] = title[18] = title[19] = '.';               // replace last few characters of long titles with "..."
          }
          s << std::left << std::setw( 23 ) << title.substr( 0, 20 ) << std::right;
          book_cart.second.pop();
        } else {
          s << std::string( 23, ' ' );
        }
      }
      s << '\n';
    }
    s << "                     " << std::string( 69, '=' ) << "\n\n\n\n";
  }
}

}  // namespace

Checkout::Checkout() : book_database_(BookDatabase::instance()) {

}

/*
 * A recursive algorithm to carefully move books from a broken cart to a working
 * cart is given as follows:
 *   START
 *   Procedure carefully_move_books (
 *       number_of_books_to_be_moved, broken_cart, working_cart, spare_cart)
 *
 *    IF number_of_books_to_be_moved == 1, THEN
 *       move top book from broken_cart to working_cart
 *       trace the move
 *
 *    ELSE
 *       carefully_move_books (
 *           number_of_books_to_be_moved-1, broken_cart, spare_cart,
 *           working_cart)
 *       move top book from broken_cart to working_cart
 *       trace the move
 *       carefully_move_books (
 *           number_of_books_to_be_moved-1, spare_cart, working_cart,
 *           broken_cart)
 *
 *    END IF
 *
 * END Procedure
 * STOP
 *
 * As a side note, the efficiency class of this algorithm is exponential.
 * That is, the Big-O is O(2^n).
 */
void Checkout::carefully_move_books(
    std::size_t quantity, std::stack<Book>& broken_cart,
    std::stack<Book>& working_cart, std::stack<Book>& spare_cart) {  
  // Implement the algorithm above.
  if (quantity == 1) { // Base case.
    working_cart.push(broken_cart.top()); // Push the top book to the working cart.
    broken_cart.pop();  // Remove the top book from the broken cart.
    trace(broken_cart, working_cart, spare_cart); // Call the trace function.
  } else {
    carefully_move_books(quantity-1, broken_cart, spare_cart, working_cart);
    working_cart.push(broken_cart.top()); // Push the top book to the working cart.
    broken_cart.pop(); // Remove the top book from the broken cart.
    trace(broken_cart, working_cart, spare_cart); // Call the trace function.
    carefully_move_books(quantity-1, spare_cart, working_cart, broken_cart);
  }
}

void Checkout::shop_for_books() {
  // Push heaviest book first and the lightest book last due to LIFO.
  my_cart_.push(Book("Hunger Games", "", "9780545310581"));
  my_cart_.push(Book("Eat pray love", "", "9780399576775"));
  my_cart_.push(Book("Les Mis", "", "0140444300"));
  my_cart_.push(Book("131 Answer Key", "", "54782169785"));
  my_cart_.push(Book("Like the Animals", "", "9780895656926"));
}

void Checkout::switch_carts() {  
  // Create a spare cart.
  std::stack<Book> spare_cart;
  // Pass arguments appropiately.
  carefully_move_books(my_cart_.size(), my_cart_, working_cart_, spare_cart);
}

void Checkout::move_books_to_checkout() {  
  // Loop through working_cart_ and push the books into the checkout_counter_.
  int original_size = working_cart_.size();
  for (int i = 0; i < original_size; ++i) {
    checkout_counter_.push(working_cart_.top()); // Add to the checkout_counter_.
    working_cart_.pop(); // Remove from the working_cart_.
  }
}

void Checkout::do_checkout() {
  // Original size of the checkout_counter_.
  int original_size = checkout_counter_.size();

  // Loop through checkout counter queue.
  for (int i = 0; i < original_size; ++i) {
    // Find and create a pointer to the book in the front of the queue.
    Book* data = book_database_.find(checkout_counter_.front().isbn());    
    if (data){ // If the book is in the database.
      amount_due_ += (*data).price(); // Add the price of the book.
      std::cout << *data << std::endl; // Print the book's data.
    } else { // When the book is not in the database.
      std::cout << "Description and Price Not Found For \"" 
        << checkout_counter_.front().isbn() << "\"! There will be no charge...\n\n";
    }
    checkout_counter_.pop(); // Removes the first book of the checkout counter queue.
  }
  std::cout << "Total: " << "$" << amount_due_ << std::endl; // Print the amount due.
}

std::stack<Book> Checkout::my_cart() const {
  return my_cart_;
}

std::stack<Book> Checkout::working_cart() const {
  return working_cart_;
}

std::queue<Book> Checkout::checkout_counter() const {
  return checkout_counter_;
}

double Checkout::amount_due() const {
  return amount_due_;
}