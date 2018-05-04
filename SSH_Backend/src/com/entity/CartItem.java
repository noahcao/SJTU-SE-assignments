package com.entity;

import org.hibernate.Session;
import org.hibernate.query.Query;

import java.util.List;

public class CartItem {
    private int id;
    private int userid;
    private int bookid;
    private int number;

    public void setId(int id) {
        this.id = id;
    }

    public void setUserid(int userid) {
        this.userid = userid;
    }

    public void setBookid(int bookid) {
        this.bookid = bookid;
    }

    public void setNumber(int number) {
        this.number = number;
    }

    public int getId() {
        return id;
    }

    public int getUserid() {
        return userid;
    }

    public int getBookid() {
        return bookid;
    }

    public int getNumber() {
        return number;
    }

    public String addCartItem() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        Query query = session.createQuery("from CartItem where userid = :userid and bookid = :bookid and number = :number");
        query.setParameter("userid", userid);
        query.setParameter("bookid", bookid);
        query.setParameter("number", number);
        List<CartItem> result = query.list();
        if(result.size() <= 0){
            setNumber(1);
            session.save(this);
            session.getTransaction().commit();
            session.close();
        }else{
            CartItem item = result.get(0);
            int num = item.getNumber() + 1;
            item.setNumber(num);
            session.update(item);
            session.getTransaction().commit();
            session.close();
        }
        return "success";
    }

    public String addOneCartItem() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        Query query = session.createQuery("from CartItem where userid = :userid and bookid =:bookid");
        query.setParameter("userid", userid);
        query.setParameter("bookid", bookid);
        CartItem item = (CartItem) query.uniqueResult();
        int num = item.getNumber() + 1;
        item.setNumber(num);
        session.update(item);
        session.getTransaction().commit();
        session.close();
        return "success";
    }

    public String subOneCartItem() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        Query query = session.createQuery("from CartItem where userid = :userid and bookid =:bookid");
        query.setParameter("userid", userid);
        query.setParameter("bookid", bookid);
        CartItem item = (CartItem) query.uniqueResult();
        int num = item.getNumber() - 1;
        if(num==0){
            session.delete(item);
            session.getTransaction().commit();
            session.close();
        }else{
            item.setNumber(num);
            session.update(item);
            session.getTransaction().commit();
            session.close();
        }
        return "success";
    }

    public String deleteCartItem() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        Query query = session.createQuery("from CartItem where userid = :userid and bookid =:bookid");
        query.setParameter("userid", userid);
        query.setParameter("bookid", bookid);
        CartItem result = (CartItem) query.uniqueResult();
        if(result != null) session.delete(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }

    public String queryCartItem() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        String query = "from CartItem where userid = :userid and bookid  = :bookid";
        List<CartItem> result = session.createQuery(query)
                .setParameter("userid", userid)
                .setParameter("bookid", bookid).list();
        if(result.size() > 0){
            CartItem item = result.get(0);
            setId(item.getId());
            setUserid(item.getUserid());
            setBookid(item.getBookid());
            setNumber(item.getNumber());
            session.getTransaction().commit();
            session.close();
        }
        session.close();
        return "success";
    }
}
