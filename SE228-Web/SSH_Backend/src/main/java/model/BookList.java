package model;

import org.hibernate.Session;

import java.util.List;

public class BookList {
    private List<Book> books;

    public List<Book> getBooks() {
        return books;
    }

    public void setBooks(List<Book> books) {
        this.books = books;
    }

    @SuppressWarnings("unchecked")
    public String queryBookList() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Book> result = session.createQuery("from Book").list();
        setBooks(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}
