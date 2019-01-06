# HW4


## Part A

### CI/CD环境搭建

CI/CD环境选择使用Drone+github搭建，在每次push之后，Drone都会对push后的内容进行构建并打包上传

#### 搭建过程

1. 根据官网教程在服务器安装Drone的0.8版本，编写相关配置文件并开启
2. 访问配置文件中设置好的IP即可访问，并登录对应的github账户
3. 开启想要监控的repo，并为止编写.drone.yml文件，.drone.yml文件包括了当监控仓库有了新的push之后需要进行的操作

以下是搭建完成后的访问界面：

![](https://i.imgur.com/twXDU8b.png)

上图中打开了对两个仓库的监控

再对一个push进行一次测试：
![](https://i.imgur.com/kasqUm5.png)

测试成功

### 准备web应用

此处准备了一个Web应用，并分别在github上建立其前后端的repo，并在drone中开启对它们的监控。
在每次前端或后端的代码push之后，通过编写相应的.drone.yml文件即可将其打包至docker hub上。

前端的.drone.yml配置如下：

``` 
workspace:
  base: /front
  path: src/github.com/XPandora/front-end
pipeline:
  build:
    image: node:latest
    commands:
      - npm install
      - npm run build
  publish:
    image: plugins/docker
    repo: xpandora/front-end
    tags: ["latest", "v2"]
    secrets: [ docker_username, docker_password ]
    dockerfile: Dockerfile
```

后端的.drone.yml配置如下：

```
workspace:
  base: /back
  path: src/github.com/XPandora/back-end
pipeline:
  build:
    image: maven:3.3.9-jdk-8-alpine
    commands:
      - mvn install
  publish:
    image: plugins/docker
    repo: xpandora/back-end
    tags: ["latest", "v2"]
    secrets: [ docker_username, docker_password ]
    dockerfile: Dockerfile
```

其中workspace制定了pipeline的工作目录，pipeline的build指定了镜像和构建的命令，publish则会发布docker镜像，其中指定了具体的repo和需要的用户名和密码
