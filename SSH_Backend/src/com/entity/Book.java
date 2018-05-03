package com.entity;

import org.hibernate.Session;

import java.util.List;

public class Book {
    private String name;
    private float price;
    private  int id;
    private String author;
    private int sales;
    private int stock;
    private String press;
    private String img;
    private int year;

    public void setImg(String img) { this.img = img; }
    public void setId(int id) { this.id = id; }
    public String getImg() { return img;}
    public float getPrice() { return price; }
    public int getId() { return id; }
    public int getSales() { return sales; }
    public int getStock() { return stock; }
    public int getYear() { return year; }
    public String getAuthor() { return author; }
    public String getName() { return name; }
    public String getPress() { return press; }
    public void setAuthor(String author) { this.author = author; }
    public void setName(String name) { this.name = name; }
    public void setPress(String press) { this.press = press; }
    public void setPrice(float price) { this.price = price; }
    public void setSales(int sales) { this.sales = sales; }
    public void setStock(int stock) { this.stock = stock; }
    public void setYear(int year) { this.year = year; }

    @SuppressWarnings("unchecked")
    public String queryBook() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Book> result = session.createQuery("from Book where id = :id").setParameter("id", id).list();
        Book item = result.get(0);
        setAuthor(item.getAuthor());
        setName(item.getName());
        setPrice(item.getPrice());
        setStock(item.getStock());
        setSales(item.getSales());
        setPress(item.getPress());
        setImg(item.getImg());
        setYear(item.getYear());
        session.getTransaction().commit();
        session.close();
        return "success";
    }


}

