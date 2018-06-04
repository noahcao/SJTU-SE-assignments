package action;

import com.opensymphony.xwork2.ActionSupport;
import model.User;
import service.AppService;

public class LogUpAction extends ActionSupport{
    private String username;
    private String password;
    private AppService appService;

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public void setAppService(AppService appService) {
        this.appService = appService;
    }

    @Override
    public String execute() throws Exception{
        User result = appService.getUserByName(username);
        if(result == null){
            User user = new User();
            user.setUsername(username);
            user.setPassword(password);
            appService.addUser(user);
        }
        else{
            setUsername(null);
        }
        return "success";
    }


}
