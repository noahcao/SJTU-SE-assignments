package model;

import org.hibernate.Session;

import java.util.List;

public class User {
    private String username;
    private String password;
    private String img;
    private Integer id;
    private int admin;

    public int getAdmin() {
        return admin;
    }

    public void setAdmin(int admin) {
        this.admin = admin;
    }

    public String getImg() {
        return img;
    }

    public Integer getId() {
        return id;
    }

    public String getPassword() {
        return password;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username){
        this.username = username;
    }

    public void setPassword(String password){
        this.password = password;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public void setImg(String img) {
        this.img = img;
    }

    public String signOn() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        User user = (User) session.createQuery("from User where username = :username")
                .setParameter("username", username).uniqueResult();
        if(user!=null){
            session.getTransaction().commit();
            session.close();
            this.setId(-1);
        }else{
            this.setAdmin(0);
            session.save(this);
            session.getTransaction().commit();
            session.close();
        }
        return "success";
    }

    public String queryUser() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<User> result = session.createQuery("from User where username = :username")
                .setParameter("username", username).list();
        User item = result.get(0);
        setAdmin(item.getAdmin());
        setId(item.getId());
        setImg(item.getImg());
        setPassword(item.getPassword());
        setUsername(item.getUsername());
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}
