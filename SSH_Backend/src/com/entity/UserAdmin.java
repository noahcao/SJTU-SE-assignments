package com.entity;

import org.hibernate.Session;

import java.util.List;

public class UserAdmin {
    private String username;
    private String password;
    private String img;
    private Integer id;

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
        session.save(this);
        session.getTransaction().commit();
        session.close();
        return "success";
    }

    public String queryUser() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<UserAdmin> result = session.createQuery("from UserAdmin where username = :username")
                .setParameter("username", username).list();
        UserAdmin item = result.get(0);
        setId(item.getId());
        setImg(item.getImg());
        setPassword(item.getPassword());
        setUsername(item.getUsername());
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}
