package action;

import com.opensymphony.xwork2.ActionSupport;
import model.Book;
import service.AppService;

import java.util.List;

public class AllBookAction extends  ActionSupport{
    private List<Book>  books;
    private AppService appService;

    public void setBooks(List<Book> books){
        this.books = books;
    }

    public List<Book> getBooks(){
        return books;
    }

    public void setAppService(AppService appService){
        this.appService = appService;
    }

    @Override
    public String execute() throws Exception{
        List<Book> books = appService.getAllBooks();
        setBooks(books);
        return "success";
    }
}
