package service.impl;

import java.util.List;

import model.*;
import org.springframework.transaction.annotation.Transactional;
import service.AppService;
import dao.BookDao;
import dao.OrderDao;
import dao.UserDao;
import dao.CartDao;

public class AppServiceImpl implements AppService{

    private BookDao bookDao;
    private OrderDao orderDao;
    private UserDao userDao;
    private CartDao cartDao;

    public CartDao getCartDao() {
        return cartDao;
    }

    public void setCartDao(CartDao cartDao) {
        this.cartDao = cartDao;
    }

    public void setBookDao(BookDao bookDao) {
        this.bookDao = bookDao;
    }

    public void setOrderDao(OrderDao orderDao) {
        this.orderDao = orderDao;
    }

    public void setUserDao(UserDao userDao) {
        this.userDao = userDao;
    }

    /* Book Operation*/
    public Integer addBook(Book book) {
        return bookDao.save(book);
    }

    public void deleteBook(Book book) {
        bookDao.delete(book);
    }

    public void updateBook(Book book) {
        bookDao.update(book);
    }

    public Book getBookById(int id) {
        return bookDao.getBookById(id);
    }

    public List<Book> getAllBooks() {
        return bookDao.getAllBooks();
    }

    /*Cart Operation*/
    public List<CartItem> getCart(int userid){
        return cartDao.getCartByUserId(userid);
    }

    public void addCartItem(CartItem item){
        cartDao.addCartItem(item);
    }

    public CartItem getCartItem(int userid, int bookid){
        return cartDao.getCartItem(userid, bookid);
    }

    public void updateCartItem(CartItem item){
        cartDao.updateCartItem(item);
    }

    public void deleteCartItem(CartItem item){
        cartDao.deleteCartItem(item);
    }

    public Cart getCartBooks(int userid){
        return cartDao.getCartBooks(userid);
    }

    /*Order Operation*/
    public Integer addOrder(Order order) {
        return orderDao.save(order);
    }

    public void deleteOrder(Order order) {
        orderDao.delete(order);
    }

    public void updateOrder(Order order) {
        orderDao.update(order);
    }

    public Order getOrderById(int id) {
        return orderDao.getOrderById(id);
    }

    public List<Order> getAllOrders() {
        return orderDao.getAllOrders();
    }

    public List<Order> getOrdersByUserId(int userid) {
        return orderDao.getOrdersByUserId(userid);
    }

    /*User Operation*/
    public Integer addUser(User user) {
        return userDao.save(user);
    }

    public void deleteUser(User user) {
        userDao.delete(user);
    }

    public void updateUser(User user) {
        userDao.update(user);
    }

    public User getUserById(int id) {
        return userDao.getUserById(id);
    }

    public User getUserByName(String name) {
        return userDao.getUserByName(name);
    }

    public List<User> getAllUsers() {
        return userDao.getAllUsers();
    }

    public User queryUser(String name, String pwd) {
        return userDao.queryUser(name, pwd);
    }

}
