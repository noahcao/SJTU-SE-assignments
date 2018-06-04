package action;

import com.opensymphony.xwork2.ActionSupport;
import model.Order;
import service.AppService;

import java.util.List;

public class QueryOrderListAction extends ActionSupport {
    private List<Order> orders;
    private int userid;

    public int getUserid() {
        return userid;
    }

    public void setUserid(int userid){
        this.userid = userid;
    }

    public List<Order> getOrders() {
        return orders;
    }

    public void setOrders(List<Order> orders) {
        this.orders = orders;
    }

    private AppService appService;

    public void setAppService(AppService appService) {
        this.appService = appService;
    }

    public String queryOrders() throws Exception{
        List<Order> result = appService.getOrdersByUserId(userid);
        this.setOrders(result);
        System.out.println("-----------------------------------------------");
        System.out.println(result);
        System.out.println("-----------------------------------------------");
        return "success";
    }
}
