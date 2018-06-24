package action;

import Icon.dao.IconDao;
import Icon.dao.impl.IconDaoImpl;
import Icon.model.IconEntity;

import org.springframework.context.ConfigurableApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;


public class UpdateIconAction {
    private int userid;
    private String img;

    public int getUserid() {
        return userid;
    }

    public void setUserid(int userid) {
        this.userid = userid;
    }

    public String getImg() {
        return img;
    }

    public void setImg(String img) {
        this.img = img;
    }

    // Update the img of user profile

    public String addImg(){
        ConfigurableApplicationContext context = null;
        context = new ClassPathXmlApplicationContext("mongo.xml");
        IconEntity icon = new IconEntity();

        System.out.println("try to inser a img into mogodb");

        icon.setId(getUserid());
        System.out.println("user id is: " + icon.getId());
        icon.setIcon(getImg());

        System.out.println("Bootstrapping HelloMongo");


        IconDao iconDao = context.getBean(IconDaoImpl.class);
        iconDao._test();

        iconDao.update(icon);

        return "success";

    }

    public String queryImg(){
        ConfigurableApplicationContext context = null;
        context = new ClassPathXmlApplicationContext("mongo.xml");

        IconDao iconDao = context.getBean(IconDaoImpl.class);
        iconDao._test();

        iconDao.createCollection();

        IconEntity e = iconDao.findOne(getUserid());
        if(e != null){
            System.out.println(e);
            System.out.println("all - id=" + e.getId() + ", icon=" + e.getIcon());
            setImg(e.getIcon());
        }
        else{
            System.out.println("No icon recorded");
        }

        System.out.println("DONE!");
        return "success";
    }

}
