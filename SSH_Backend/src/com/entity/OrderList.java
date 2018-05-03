package com.entity;

import org.hibernate.Session;

import java.util.List;

public class OrderList {
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

    public void setBooks(List<Order> orders) {
        this.orders = orders;
    }

    @SuppressWarnings("unchecked")
    public String queryOrderList() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Order> result = session.createQuery("from Order where userid =:userid")
                .setParameter("userid", userid).list();
        setBooks(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}
