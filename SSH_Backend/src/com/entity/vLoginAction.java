package com.entity;

import com.opensymphony.xwork2.ActionSupport;

public class vLoginAction extends ActionSupport {
    private String tip;
    private String forwardUrl;

    public String getTip() {
        return tip;
    }

    public void setTip(String tip) {
        this.tip = tip;
    }

    public String getForwardUrl() {
        return forwardUrl;
    }

    public void setForwardUrl(String forwardUrl) {
        this.forwardUrl = forwardUrl;
    }

    public String execute() throws Exception
    {
        System.out.println("execute run");
        return "fail";
    }

    public String vprocesslogin() throws Exception {
        System.out.println("tip: " + tip);
        return SUCCESS;
    }
}
