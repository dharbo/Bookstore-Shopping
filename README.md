# Bookstore Shopping
This group project resembles bookstore shopping with a cart. Essentially, the last book you place onto your book cart will be on top and 
will be the first book you remove, similarly to a stack data structure. If something from the bottom is to be removed, everything above it must be 
removed first. For this implementation we stack the books with the most fragile at the top, so if the current cart is broken, we move the books into
one cart, and then into another to keep the most fragile at the top. Once ready, the customer would enter the checkout line queue, and then each book would be
scanned and the ISBN is used to query the store’s persistent database for the book’s full title, author, and price. A total is given based on the
books in the database.
