package action;

import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import model.CartItem;
import service.AppService;

import java.util.Iterator;
import java.util.Map;
import java.util.Set;


public class UpdateCartItemAction extends ActionSupport{
    private int id;
    private int userid;
    private int bookid;
    private int number;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
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

    private AppService appService;
    public void setAppService(AppService appService){
        this.appService = appService;
    }

    /*
    public String deleteCartItem() throws Exception{
        Map<String, Object> usersession = ActionContext.getContext().getSession();
        Set<CartItem> cart = (Set<CartItem>) usersession.get("cart");
        if (cart == null) return "error";
        Iterator it = cart.iterator();
        while(it.hasNext()){
            CartItem next = (CartItem) it.next();
            if(next.getBookid() == this.bookid){
                it.remove();
                usersession.replace("cart", cart);
                return "success";
            }
        }
        return "error";
    }*/

    public String updateCartItem() throws Exception {
        CartItem item = appService.getCartItem(userid, bookid);
        if (item != null) {
            int newnumber = item.getNumber() + this.number;
            if(newnumber > 0){
                item.setNumber(item.getNumber() + this.number);
                appService.updateCartItem(item);
            }
            else{
                appService.deleteCartItem(item);
            }
        } else {
            CartItem newitem = new CartItem();
            newitem.setNumber(this.number);
            newitem.setUserid(this.userid);
            newitem.setBookid(this.bookid);
            appService.addCartItem(newitem);
        }
        return "success";
    }

    public String deleteCartItem() throws Exception{
        CartItem item = appService.getCartItem(userid, bookid);
        if(item != null){
            appService.deleteCartItem(item);
        }
        return "success";
    }
}
