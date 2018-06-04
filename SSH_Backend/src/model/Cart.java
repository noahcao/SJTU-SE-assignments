package model;

import org.hibernate.Session;
import org.hibernate.query.Query;

import java.util.ArrayList;
import java.util.List;

public class Cart {
    private List<Book> BooksInCart;
    private int userid;

    public List<Book> getBooksInCart() {
        return BooksInCart;
    }

    public void setBooksInCart(List<Book> booksInCart) {
        BooksInCart = booksInCart;
    }

    public int getUserid() {
        return userid;
    }

    public void setUserid(int userid) {
        this.userid = userid;
    }

    public String queryCart() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<CartItem> result = session.createQuery("from CartItem where userid = :userid")
                .setParameter("userid", userid).list();
        List<Book> bookList = new ArrayList<>();
        for(int i = 0; i < result.size(); i++){
            System.out.println(result.get(i));
            int bookid = result.get(i).getBookid();
            System.out.println("--------------------------------------------------------------------------");
            System.out.println(bookid);
            System.out.println("--------------------------------------------------------------------------");
            Query query = session.createQuery("from Book where id = :bookid");
            query.setParameter("bookid", bookid);
            Book book = (Book)query.uniqueResult();
            book.setNumber(result.get(i).getNumber());
            bookList.add(book);
        }
        setBooksInCart(bookList);
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}
