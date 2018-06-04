package model;

import org.hibernate.Session;

import java.util.List;

public class Book {
    private String name;
    private float price;
    private  int id;
    private String author;
    private int sales;
    private int number;
    private String press;
    private String img;
    private int year;

    public Book(){ }

    public Book(String name, float price, String author, String press,
                int year){
        this.name = name;
        this.price = price;
        this.author = author;
        this.press = press;
        this.year = year;
        this.sales = 0;
    }

    public void setImg(String img) { this.img = img; }
    public void setId(int id) { this.id = id; }
    public String getImg() { return img;}
    public float getPrice() { return price; }
    public int getId() { return id; }
    public int getSales() { return sales; }
    public int getNumber() { return number; }
    public int getYear() { return year; }
    public String getAuthor() { return author; }
    public String getName() { return name; }
    public String getPress() { return press; }
    public void setAuthor(String author) { this.author = author; }
    public void setName(String name) { this.name = name; }
    public void setPress(String press) { this.press = press; }
    public void setPrice(float price) { this.price = price; }
    public void setSales(int sales) { this.sales = sales; }
    public void setNumber(int stock) { this.number = stock; }
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
        setNumber(item.getNumber());
        setSales(item.getSales());
        setPress(item.getPress());
        setImg(item.getImg());
        setYear(item.getYear());
        session.getTransaction().commit();
        session.close();
        return "success";
    }

    public String addBook() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        session.save(this);
        session.getTransaction().commit();
        session.close();
        return "success";
    }

    public String delBook() throws Exception{
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        Book item = (Book)session.createQuery("from Book where id = :id")
                .setParameter("id", id).uniqueResult();
        if(item != null){
            session.delete(item);
            session.getTransaction().commit();
            session.close();
        }
        return "success";
    }

}

