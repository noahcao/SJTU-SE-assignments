package com.entity;

import org.hibernate.Session;

import java.util.List;

public class Cart {
    private List<Item> Cart;
    private int userid;

    public int getUserid() {
        return userid;
    }

    public void setUserid(int userid) {
        this.userid = userid;
    }

    private class Item{
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
    }

    public List<Item> getCart() {
        return Cart;
    }

    public void setCart(List<Item> cart) {
        Cart = cart;
    }

    public String queryCart() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Item> result = session.createQuery("from Cart where userid = :userid")
                .setParameter("userid", userid).list();
        setCart(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}
