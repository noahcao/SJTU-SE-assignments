package action;

import com.opensymphony.xwork2.ActionSupport;
import model.Book;
import service.AppService;
import model.CartItem;

import java.util.ArrayList;
import java.util.List;
import model.Cart;

public class QueryCartAction extends ActionSupport {
    private List<Book> BooksInCart;
    private int userid;
    private List<CartItem> Carts;

    public List<CartItem> getCarts() {
        return Carts;
    }

    public void setCarts(List<CartItem> carts) {
        Carts = carts;
    }

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

    public String queryCart(){
        Cart cart = appService.getCartBooks(this.getUserid());
        setCarts(cart.getBookCarts());
        System.out.println("cartssss: \n" + Carts);

        setBooksInCart(cart.getBooksInCart());
        System.out.println("got cart: " + cart.getBooksInCart());
        System.out.println("got cart: " + cart.getUserid());
        return "success";
    }
}
