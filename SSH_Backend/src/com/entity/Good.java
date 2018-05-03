package com.entity;

import com.opensymphony.xwork2.ActionSupport;
import org.hibernate.Session;

import java.util.List;

public class Good extends ActionSupport {
    private int id;

    private String name;
    private String author;
    private String summary;
    private float price;
    private int stock;
    private int year;

    public Good() {
    }

    public float getPrice() {
        return price;
    }

    public int getStock() {
        return stock;
    }

    public int getYear() {
        return year;
    }

    public int getId() {
        return id;
    }

    public String getAuthor() {
        return author;
    }

    public String getName() {
        return name;
    }

    public String getSummary() {
        return summary;
    }

    public void setAuthor(String author) {
        this.author = author;
    }

    public void setId(int id) {
        this.id = id;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setPrice(float price) {
        this.price = price;
    }

    public void setStock(int stock) {
        this.stock = stock;
    }

    public void setSummary(String summary) {
        this.summary = summary;
    }

    public void setYear(int year) {
        this.year = year;
    }

    @SuppressWarnings("unchecked")
    public String queryItem() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Good> result = session.createQuery("from Good where id = :id").setParameter("id", id).list();
        Good item = result.get(0);
        setAuthor(item.getAuthor());
        setName(item.getName());
        setPrice(item.getPrice());
        setStock(item.getStock());
        setSummary(item.getSummary());
        setYear(item.getYear());
        session.getTransaction().commit();
        session.close();
        return "success";
    }

}
