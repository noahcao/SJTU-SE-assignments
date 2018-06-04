package dao.impl;

import org.springframework.orm.hibernate5.support.HibernateDaoSupport;

import dao.CartDao;

import model.CartItem;
import model.Cart;
import model.Book;

import java.util.ArrayList;
import java.util.List;

public class CartDaoImpl extends HibernateDaoSupport implements CartDao {
    public List<CartItem> getCartByUserId(int userid){
        @SuppressWarnings("unchecked")
        List<CartItem> cart = (List<CartItem>) getHibernateTemplate().find(
                "from CartItem as c where c.userid=?", userid);
        return cart;
    }

    public Integer addCartItem(CartItem item){ return (Integer) getHibernateTemplate().save(item);}

    public void updateCartItem(CartItem item){ getHibernateTemplate().merge(item);}

    public void deleteCartItem(CartItem item){ getHibernateTemplate().delete(item);}

    public CartItem getCartItem(int userid, int bookid){
        @SuppressWarnings("unchecked")
        List<CartItem> items = (List<CartItem>) getHibernateTemplate().find(
                "from CartItem as b where b.userid=? and b.bookid=?", userid, bookid);
        CartItem item = items.size() >0 ? items.get(0) : null;
        return item;
    }

    public Book getBookById(int id) {
        @SuppressWarnings("unchecked")
        List<Book> books = (List<Book>) getHibernateTemplate().find(
                "from Book as b where b.id=?", id);
        Book book = books.size() > 0 ? books.get(0) : null;
        return book;
    }

    public Cart getCartBooks(int userid){
        @SuppressWarnings("unchecked")
        List<CartItem> items = (List<CartItem>) getHibernateTemplate().find(
            "from CartItem as c where c.userid=?", userid);
        Cart cart = new Cart();
        List<Book> books = new ArrayList<>();
        if(items == null){
            cart.setUserid(userid);
            cart.setBooksInCart(books);
        }
        else{
            for(int i = 0; i < items.size(); i++){
                CartItem item = items.get(i);
                int bookid = item.getBookid();
                @SuppressWarnings("unchecked")
                List<Book> getbooks = (List<Book>) getHibernateTemplate().find(
                        "from Book as b where b.id=?", bookid);
                Book book = books.size() > 0 ? getbooks.get(0) : null;
                if(book != null){
                    books.add(book);
                }
            }
            cart.setUserid(userid);
            cart.setBooksInCart(books);
        }
        return cart;
    }
}
