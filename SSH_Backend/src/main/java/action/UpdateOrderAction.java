package action;

import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import model.*;
import model.Book;
import org.hibernate.Hibernate;
import org.hibernate.Session;
import org.hibernate.query.Query;
import service.AppService;

import java.sql.Timestamp;
import java.util.*;

public class UpdateOrderAction extends ActionSupport{
    private int orderid;
    private int userid;
    private int bookid;
    private int number;
    private Timestamp time;
    private float price;
    private String bookname;

    public int getOrderid() {
        return orderid;
    }

    public void setOrderid(int orderid) {
        this.orderid = orderid;
    }

    public int getUserid() {
        return userid;
    }

    public void setUserid(int userid) {
        this.userid = userid;
    }

    public int getBookid() {
        return bookid;
    }

    public void setBookid(int bookid) {
        this.bookid = bookid;
    }

    public int getNumber() {
        return number;
    }

    public void setNumber(int number) {
        this.number = number;
    }

    public Timestamp getTime() {
        return time;
    }

    public void setTime(Timestamp time) {
        this.time = time;
    }

    public float getPrice() {
        return price;
    }

    public void setPrice(float price) {
        this.price = price;
    }

    public String getBookname() {
        return bookname;
    }

    public void setBookname(String bookname) {
        this.bookname = bookname;
    }

    private AppService appService;

    public void setAppService(AppService appService) {
        this.appService = appService;
    }

    public String addOrder() throws Exception{
        Timestamp time = new Timestamp(new Date().getTime());

        User result = appService.getUserById(this.userid);
        if (result == null) {
            return "error";
        }

        List<CartItem> booksInCart = appService.getCart(this.userid);
        for(int i = 0; i < booksInCart.size(); i++){
            CartItem item = booksInCart.get(i);
            int bookid = item.getBookid();
            Book book = (Book) appService.getBookById(bookid);
            if(book == null){
                return "error";
            }
            else{
                // find the book
                Order neworder = new Order();
                neworder.setPrice(item.getNumber() * book.getPrice());
                neworder.setTime(time);
                neworder.setUserid(this.userid);
                neworder.setBookname(book.getName());
                neworder.setNumber(item.getNumber());
                neworder.setBookid(book.getId());
                appService.addOrder(neworder);
                appService.deleteCartItem(item);

                book.setNumber(book.getNumber() - neworder.getNumber());
                book.setSales(book.getSales() + neworder.getNumber());
                appService.updateBook(book);
            }
        }
        return "success";
    }
}
