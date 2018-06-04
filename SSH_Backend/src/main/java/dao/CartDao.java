package dao;

import java.util.List;

import model.Cart;
import model.CartItem;

public interface CartDao {
    public List<CartItem> getCartByUserId(int userid);

    // public Cart getCartListByUserId(int userid);

    public CartItem getCartItem(int userid, int bookid);

    public Integer addCartItem(CartItem item);

    public void deleteCartItem(CartItem item);

    public void updateCartItem(CartItem item);

    public Cart getCartBooks(int userid);
}
