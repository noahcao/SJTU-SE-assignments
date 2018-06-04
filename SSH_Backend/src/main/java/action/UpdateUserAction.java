package action;

import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import model.Order;
import model.Cart;
import model.User;
import org.hibernate.Hibernate;
import service.AppService;

import java.util.HashSet;
import java.util.Set;

public class UpdateUserAction {
    private int id;
    private String password;
    private String username;
    private int admin;

    public int getAdmin() {
        return admin;
    }

    public void setAdmin(int admin) {
        this.admin = admin;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public void setUserInfo(User user){
        setUsername(user.getUsername());
        setPassword(user.getPassword());
        setId(user.getId());
        setAdmin(user.getAdmin());
    }

    private AppService appService;

    public void setAppService(AppService appService) {
        this.appService = appService;
    }

    public String queryUser() throws Exception{
        System.out.println("----------------------------------------------");
        System.out.println(this.username);
        System.out.println("----------------------------------------------");
        User result = appService.getUserByName(this.username);
        if(result != null){
            setUserInfo(result);
        }
        else{
            setAdmin(-1);
        }
        return "success";
    }

    public String update() throws Exception{
        User result = appService.getUserById(id);
        if(result != null){
            result.setUsername(username);
            result.setPassword(password);
            appService.updateUser(result);
            return "success";
        }
        return "error";
    }

    public String signOn() throws Exception {
        User result = appService.getUserByName(username);
        if(result == null){
            this.setAdmin(0);
            User newuser = new User();
            newuser.setPassword(this.getPassword());
            newuser.setUsername(this.getUsername());
            newuser.setAdmin(this.getAdmin());
            appService.addUser(newuser);
        }else{
            this.setId(-1);
        }
        return "success";
    }
}
