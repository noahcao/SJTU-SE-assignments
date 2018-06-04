package action;

import com.opensymphony.xwork2.ActionSupport;
import model.Book;
import service.AppService;

import java.util.ArrayList;
import java.util.List;
import model.Cart;

public class QueryCartAction extends ActionSupport {
    private List<Book> BooksInCart;
    private int userid;

    public List<Book> getBooksInCart() {
        return BooksInCart;
    }

    public void setBooksInCart(List<Book> booksInCart) {
        BooksInCart = booksInCart;
    }

    public int getUserid() {
        return userid;
    }

    public void setUserid(int userid) {
        this.userid = userid;
    }

    private AppService appService;

    public void setAppService(AppService appService) {
        this.appService = appService;
    }

    public String queryCart(int userid){
        System.out.println("--------13123213123123123123123123123123123123123123");
        System.out.println("--------13123213123123123123123123123123123123123123");
        Cart cart = appService.getCartBooks(userid);
        setBooksInCart(cart.getBooksInCart());
        return "success";
    }
}
