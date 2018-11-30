package action;

import com.opensymphony.xwork2.ActionSupport;
import model.Book;
import service.AppService;

public class UpdateBookAction extends ActionSupport{
    private String name;
    private float price;
    private  int id;
    private String author;
    private int sales;
    private int number;
    private String press;
    private String img;
    private int year;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public float getPrice() {
        return price;
    }

    public void setPrice(float price) {
        this.price = price;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getAuthor() {
        return author;
    }

    public void setAuthor(String author) {
        this.author = author;
    }

    public int getSales() {
        return sales;
    }

    public void setSales(int sales) {
        this.sales = sales;
    }

    public int getNumber() {
        return number;
    }

    public void setNumber(int number) {
        this.number = number;
    }

    public String getPress() {
        return press;
    }

    public void setPress(String press) {
        this.press = press;
    }

    public String getImg() {
        return img;
    }

    public void setImg(String img) {
        this.img = img;
    }

    public int getYear() {
        return year;
    }

    public void setYear(int year){
        this.year = year;
    }

    private AppService appService;

    public void setAppService(AppService appService) {
        this.appService = appService;
    }

    public String queryBook() throws Exception{
        Book item = appService.getBookById(id);
        if(item != null){
            setAuthor(item.getAuthor());
            setName(item.getName());
            setPrice(item.getPrice());
            setSales(item.getSales());
            setPress(item.getPress());
            setImg(item.getImg());
            setYear(item.getYear());
            setNumber(item.getNumber());
            return "success";
        }
        return "fail";
    }

    public String delBook() throws Exception{
        System.out.println("--------------------- delte a book -------------" + this.id);
        Book item = appService.getBookById(this.id);
        appService.deleteBook(item);
        return "success";
    }

    public String addBook() throws Exception{
        Book item = new Book();
        item.setNumber(getNumber());
        item.setSales(getSales());
        item.setAuthor(getAuthor());
        item.setImg(getImg());
        item.setName(getName());
        item.setPress(getPress());
        item.setYear(getYear());
        item.setPrice(getPrice());
        appService.addBook(item);
        return "success";
    }
}
