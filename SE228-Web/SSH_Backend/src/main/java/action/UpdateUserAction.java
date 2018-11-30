package action;

import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import model.Order;
import model.Cart;
import model.User;
import org.hibernate.Hibernate;
import service.AppService;

import java.util.HashSet;
import java.util.List;

public class UpdateUserAction {
    private int id;
    private String password;
    private String username;
    private int admin;
    private List<User> userList;

    public List<User> getUserList() {
        return userList;
    }

    public void setUserList(List<User> userList) {
        this.userList = userList;
    }

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
        System.out.println(this.username);
        User result = appService.getUserByName(this.username);
        if(result != null){
            setUserInfo(result);
        }
        else{
            setAdmin(-1);
        }
        return "success";
    }

    public String queryUserList() throws Exception{
        List<User> userlist = appService.getAllUsers();
        setUserList(userlist);
        return "success";
    }

    public String update() throws Exception{
        User result = appService.getUserById(id);
        System.out.println("fdsfsdsfas");
        System.out.println(result);
        if(result != null){
            result.setPassword(password);
            appService.updateUser(result);
            return "success";
        }
        return "error";
    }

    public String banfree() throws Exception{
        User result = appService.getUserById(id);
        if(result != null){
            result.setAdmin(admin);
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
