package com.entity;

import org.hibernate.Session;
import org.hibernate.query.Query;

import java.sql.Timestamp;
import java.util.List;

public class Order {
    private int orderid;
    private int userid;
    private int bookid;
    private int number;
    private Timestamp time;
    private float price;
    private String bookname;

    public String getBookname() {
        return bookname;
    }

    public void setBookname(String bookname) {
        this.bookname = bookname;
    }

    public Order(){

    }

    public int getNumber() {
        return number;
    }

    public void setNumber(int number) {
        this.number = number;
    }

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
    public String addOrder() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        Timestamp time = new Timestamp(System.currentTimeMillis());
        List<CartItem> result = session.createQuery("from CartItem where userid = :userid")
                .setParameter("userid", userid).list();
        for(int i = 0; i < result.size(); i++){
            int bookid = result.get(i).getBookid();
            Query query = session.createQuery("from Book where id = :bookid");
            query.setParameter("bookid", bookid);
            Book book = (Book)query.uniqueResult();
            Order order = new Order();
            order.setPrice(result.get(i).getNumber() * book.getPrice());
            order.setTime(time);
            order.setBookname(book.getName());
            order.setNumber(result.get(i).getNumber());
            order.setBookid(book.getId());
            order.setUserid(result.get(i).getUserid());
            session.save(order);
            book.setSales(book.getSales() + result.get(i).getNumber());
            session.update(book);
            session.delete(result.get(i));
        }
        session.getTransaction().commit();
        session.close();
        return "success";
    }

    public String queryOrder() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Order> result = session.createQuery("from Order where userid = :userid").
                setParameter("userid", userid).list();
        Order item = result.get(0);
        setNumber(item.getNumber());
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
