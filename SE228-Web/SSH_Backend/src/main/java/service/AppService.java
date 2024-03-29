package service;

import java.util.List;

import model.Book;
import model.CartItem;
import model.Order;
import model.User;
import model.Cart;
import Icon.model.IconEntity;


/**
 * @author seniyuting
 * @version 1.0
 *
 */
public interface AppService {

    /**
     * book
     *
     */
    public Integer addBook(Book book);

    public void deleteBook(Book book);

    public void updateBook(Book book);

    public Book getBookById(int id);

    public List<Book> getAllBooks();

    /**
     * order
     *
     */
    public Integer addOrder(Order order);

    public void deleteOrder(Order order);

    public void updateOrder(Order order);

    public Order getOrderById(int id);

    public List<Order> getAllOrders();

    public List<Order> getOrdersByUserId(int userid);

    /**
     * cart
     * */

    public List<CartItem> getCart(int userid);

    public void updateCartItem(CartItem item);

    public CartItem getCartItem(int userid, int bookid);

    public void addCartItem(CartItem item);

    public void deleteCartItem(CartItem item);

    public Cart getCartBooks(int userid);
    /**
     * user
     *
     */
    public Integer addUser(User user);

    public void deleteUser(User user);

    public void updateUser(User user);

    public User getUserById(int id);

    public User getUserByName(String name);

    public List<User> getAllUsers();

    public User queryUser(String username, String password);

    /* icon */
    public void insertImg(IconEntity icon);

    public IconEntity getImg(int userid);

    public void updateImg(IconEntity icon);

}