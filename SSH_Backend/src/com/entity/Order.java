package com.entity;

import org.hibernate.Session;

import java.sql.Timestamp;
import java.util.List;

public class Order {
    private int orderid;
    private int userid;
    private int bookid;
    private Timestamp time;
    private float price;

    public Timestamp getTime() {
        return time;
    }

    public void setTime(Timestamp time) {
        this.time = time;
    }

    public int getOrderid() {
        return orderid;
    }

    public void setOrderid(int order_id) {
        this.orderid = order_id;
    }

    public int getUserid() {
        return userid;
    }

    public void setUserid(int user_id) {
        this.userid = user_id;
    }

    public int getBookid() {
        return bookid;
    }

    public void setBookid(int book_id) {
        this.bookid = book_id;
    }


    public float getPrice() {
        return price;
    }

    public void setPrice(float price) {
        this.price = price;
    }

    @SuppressWarnings("unchecked")
    public String queryOrder() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Order> result = session.createQuery("from Order where userid = :userid").
                setParameter("userid", userid).list();
        Order item = result.get(0);
        setOrderid(item.getOrderid());
        setBookid(item.getBookid());
        setPrice(item.getPrice());
        setTime(item.getTime());
        setUserid(item.getUserid());
        session.getTransaction().commit();
        session.close();
        return "success";
    }

}
