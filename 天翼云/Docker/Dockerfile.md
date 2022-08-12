# 使用Dockerfile定制镜像

Dockerfile 是一个文本文件，其内包含了一条条的 **指令(Instruction)**，**每一条指令构建一层**，因此每一条指令的内容，就是描述该层应当如何构建。

## FROM指定基础镜像

`FROM`是Dockerfile中必备的指令，**且必须是第一条**。有两种选择

- `FROM [基础镜像]`	从基础镜像开始定制
- `FROM scratch`   从空白镜像开始定制

## RUN执行命令

`RUN`是用来执行命令行命令的。其格式有两种：

- shell格式：`RUN <命令>`

```dockerfile
RUN echo '<h1>Hello, Docker!</h1>' > /usr/share/nginx/html/index.html
```

- exec格式：`RUN ["可执行文件", "参数1", "参数2"]`，这更像是函数调用中的格式。

> 注意：每一个 `RUN` 的行为，都会新建立一层，在其上执行这些命令，执行结束后，`commit` 这一层的修改，构成新的镜像。
>
> 因此，对于同一层中的命令建议用 `&&` 将各个命令串联起来

在命令最后添加清理工作的命令，删除为了编译构造所需的软件，防止镜像变得臃肿

## 构建镜像

使用命令`docker build`

```bash
docker build [选项] <上下文路径/URL/->
# - 表示通过STDIN给出Dockerfile或上下文
```

在 `Dockerfile` 文件所在目录执行：

```bash
[root@centos1 mynginx]# docker build -t nginx:v3 .
Sending build context to Docker daemon  2.048kB
Step 1/2 : FROM nginx
 ---> 55f4b40fe486
Step 2/2 : RUN echo '<h1>Hello, Docker! by Dockerfile</h1>' > /usr/share/nginx/html/index.html
 ---> Running in 4452c01a8ba7
Removing intermediate container 4452c01a8ba7
 ---> 9531517b7769
Successfully built 9531517b7769
Successfully tagged nginx:v3
```

## 镜像构建上下文（Context）

首先Docker是C/S模式设计，像`docker build`这样的命令只是客户端的请求，实际完成镜像构建的是docker的服务端守护进程。

要让服务器端获取本地文件就需要用户指定构建镜像的上下文路径，`docker build` 命令得知这个路径后，会将路径下的所有内容打包，然后上传给 Docker 引擎。这样 Docker 引擎收到这个上下文包后，展开就会获得构建镜像所需的一切文件。

如果在 `Dockerfile` 中这么写：

```dockerfile
COPY ./package.json /app/
```

这并不是要复制执行 `docker build` 命令所在的目录下的 `package.json`，也不是复制 `Dockerfile` 所在目录下的 `package.json`，而是复制 **上下文（context）** 目录下的 `package.json`。

因此，`COPY` 这类指令中的源文件的路径都是*相对路径*。

现在就可以理解刚才的命令 `docker build -t nginx:v3 .` 中的这个 `.`，实际上是在指定上下文的目录，`docker build` 命令会将该目录下的内容打包交给 Docker 引擎以帮助构建镜像。

如果观察 `docker build` 输出，我们其实已经看到了这个发送上下文的过程：

```bash
[root@centos1 mynginx]# docker build -t nginx:v3 .
Sending build context to Docker daemon  2.048kB
...
```

一般来说，应该会将 `Dockerfile` 置于一个空目录下，或者项目根目录下。如果该目录下没有所需文件，那么应该把所需文件复制一份过来。如果目录下有些东西确实不希望构建时传给 Docker 引擎，那么可以用 `.gitignore` 一样的语法写一个 `.dockerignore`，该文件是用于剔除不需要作为上下文传递给 Docker 引擎的。



- 使用`-f`指定不同的dockerfile位置并指定上下文发送到Docker 守护进程

```bash
 mkdir -p dockerfiles context
 mv Dockerfile dockerfiles && mv hello context
 docker build --no-cache -t helloapp:v2 -f dockerfiles/Dockerfile context
```

- 使用`-`指示Dockerfile从STDIN读取，并且上下文中没有文件可供复制

```bash
docker build [OPTIONS] -
```

注：此时使用COPY会报错

- 从本地上下文构建，并使用STDIN传入Dockerfile

```bash
docker build [OPTIONS] -f- PATH
```

下面的例子将本地文件夹作为上下文，并用STDIN传入dockerfile

```bash
# create a directory to work in
mkdir example
cd example

# create an example file
touch somefile.txt

# build an image using the current directory as context, and a Dockerfile passed through stdin
docker build -t myimage:latest -f- . <<EOF
FROM busybox
COPY somefile.txt ./
RUN cat /somefile.txt
EOF
```

- 或者可以用远程`git`作为上下文，并用STDIN传入dockerfile

```bash
docker build -t myimage:latest -f- https://github.com/docker-library/hello-world.git <<EOF
FROM busybox
COPY hello.c ./
EOF
```



---

---



# Docker指令详解

## COPY 复制文件

格式：

- `COPY [--chown=<user>:<group>] <源路径>... <目标路径>`			类似于命令行
- `COPY [--chown=<user>:<group>] ["<源路径1>",... "<目标路径>"]`    类似于函数调用

包括空格的路径需要第二种形式

> `--chown`只能用于构建linux容器

`COPY` 指令将从**构建上下文目录**中 `<源路径>` 的文件/目录复制到新的一层的镜像内的 `<目标路径>` 位置。

`<源路径>` 可以是多个，甚至可以是通配符，其通配符规则要满足 Go 的 [`filepath.Match`](https://golang.org/pkg/path/filepath/#Match) 规则，如：

```dockerfile
COPY hom* /mydir/		#将上下文目录中前缀为"hom"的文件复制到 绝对路径 “/mydir/”
COPY hom?.txt /mydir/	#'?'可以是任意单个字符，如"home.txt"
```

`<目标路径>` 可以是容器内的绝对路径，也可以是相对于工作目录的相对路径（工作目录可以用 `WORKDIR` 指令来指定）。目标路径不需要事先创建，如果目录不存在会在复制文件前先行创建缺失目录。

```dockerfile
COPY test.txt relativeDir/		#把"test.txt"加入 "<WORKDIR>/relativeDir/"
COPY test.txt /absoluteDir/		#把"test.txt"加入 "/absoluteDir/"
COPY arr[[]0].txt /mydir/		#复制具有特殊符号的文件（如"arr[0].txt"），需要一句golang规则转义
```

此外，还需要注意一点，使用 `COPY` 指令，源文件的各种元数据都会保留。比如读、写、执行权限、文件变更时间等。这个特性对于镜像定制很有用。特别是构建相关文件都在使用 Git 进行管理的时候。

在使用该指令的时候还可以加上 `--chown=<user>:<group>` 选项来改变文件的所属用户及所属组。

```dockerfile
#如果提供了用户名或组名，则容器的根文件系统 /etc/passwd和/etc/group文件将分别用于执行从名称到整数 UID 或 GID 的转换。
COPY --chown=55:mygroup files* /mydir/
#提供不带组名的用户名或不带 GID 的 UID 将使用与 GID 相同的数字 UID。
COPY --chown=bin files* /mydir/			
COPY --chown=1 files* /mydir/
COPY --chown=10:11 files* /mydir/
```

如果容器根文件系统`/etc/passwd`不包含文件，并且标志中使用了用户名或组名， 则构建将在操作中失败。使用数字 ID 不需要查找并且不依赖于容器根文件系统内容。

> 如果使用 STDIN ( `docker build - < somefile`) 构建，则没有构建上下文，因此`COPY`无法使用。

可选择`COPY`接受一个标志，该标志`--from=<name>`可用于将源位置设置为先前的构建阶段（使用创建`FROM .. AS <name>`），而不是用户发送的构建上下文。如果找不到具有指定名称的构建阶段，则尝试使用具有相同名称的图像。

`COPY`遵守以下规则：

- `<src>`路径必须在构建的*上下文中；*你不能`COPY ../something /something`，因为`docker build`的第一步 是将上下文目录（和子目录）发送到 docker 守护进程。
- 如果`<src>`是目录，则复制目录的全部内容，包括文件系统元数据。（目录本身没有被复制，只是它的内容）

- 如果`<src>`是任何其他类型的文件，则将其与其元数据一起单独复制。在这种情况下，如果`<dest>`以斜杠结尾`/`，它将被视为一个目录，其内容`<src>`将写入`<dest>/base(<src>)`.

- `<src>`如果直接或由于使用通配符而指定了多个资源，则`<dest>`必须是目录，并且必须以斜杠结尾`/`。

- 如果`<dest>`不以斜杠结尾，则将其视为常规文件，其内容`<src>`将写入`<dest>`.

- 如果`<dest>`不存在，它会连同其路径中所有缺失的目录一起创建。

---

## ADD更高级的复制文件

`ADD` 指令和 `COPY` 的格式和性质基本一致。但是在 `COPY` 基础上增加了一些功能。

- 不实用、不推荐的功能。`<源路径>可以是一个URL`，Docker引擎会试图下载连接的文件到`<目标路径>`去，默认权限`600`。如果不想要这个权限，需要额外一层`RUN`去修改；如果下载的是压缩包，需要解压缩也要额外一层`RUN`。这样还不如直接用`RUN`使用`wget`或者`curl`工具下载，然后修改权限、解压缩、清理无用文件一步到位。

- 比较实用的功能。如果 `<源路径>` 为一个 `tar` 压缩文件的话，压缩格式为 `gzip`, `bzip2` 以及 `xz` 的情况下，`ADD` 指令将会自动解压缩这个压缩文件到 `<目标路径>` 去。

  在某些情况下，这个自动解压缩的功能非常有用，比如官方镜像 `ubuntu` 中：

```dockerfile
FROM scratch
ADD ubuntu-xenial-core-cloudimg-amd64-root.tar.gz /
...
```

但在某些情况下，如果我们真的是希望复制个压缩文件进去，而不解压缩，这时就不可以使用 `ADD` 命令了。

另外需要注意的是，`ADD` 指令会令镜像构建缓存失效，从而可能会令镜像构建变得比较缓慢。

因此在 `COPY` 和 `ADD` 指令中选择的时候，可以遵循这样的原则，所有的文件复制均使用 `COPY` 指令，仅在需要自动解压缩的场合使用 `ADD`。

在使用该指令的时候还可以加上 `--chown=<user>:<group>` 选项来改变文件的所属用户及所属组。

---

## CMD 容器启动命令

`CMD` 指令是用于指定默认的容器主进程的启动命令的。

`CMD` 指令的格式和 `RUN` 相似，也是两种格式：

- `shell` 格式：`CMD <命令>`
- `exec` 格式：`CMD ["可执行文件", "参数1", "参数2"...]`
- 参数列表格式：`CMD ["参数1", "参数2"...]`。在指定了 `ENTRYPOINT` 指令后，用 `CMD` 指定具体的参数。

Docker 是进程，那么在启动容器的时候，需要指定所运行的程序及参数。

在运行时可以指定新的命令来替代镜像设置中的这个默认命令，比如，`ubuntu` 镜像默认的 `CMD` 是 `/bin/bash`，如果我们直接 `docker run -it ubuntu` 的话，会直接进入 `bash`。我们也可以在运行时指定运行别的命令，如 `docker run -it ubuntu cat /etc/os-release`。这就是用 `cat /etc/os-release` 命令替换了默认的 `/bin/bash` 命令了，输出了系统版本信息。

在指令格式上，一般推荐使用 `exec` 格式，这类格式在解析时会被解析为 JSON 数组，因此一定要使用双引号 `"`，而不要使用单引号。

如果使用 `shell` 格式的话，实际的命令会被包装为 `sh -c` 的参数的形式进行执行。比如：

```dockerfile
CMD echo $HOME
```

在实际执行中，会将其变更为：

```dockerfile
CMD [ "sh", "-c", "echo $HOME" ]
```

这就是为什么我们可以使用环境变量的原因，因为这些环境变量会被 shell 进行解析处理。

提到 `CMD` 就不得不提容器中应用在前台执行和后台执行的问题。这是初学者常出现的一个混淆。

Docker 不是虚拟机，容器中的应用都应该以前台执行，而不是像虚拟机、物理机里面那样，用 `systemd` 去启动后台服务，容器内没有后台服务的概念。

一些初学者将 `CMD` 写为：

```dockerfile
CMD service nginx start
```

然后发现容器执行后就立即退出了。甚至在容器内去使用 `systemctl` 命令结果却发现根本执行不了。这就是因为没有搞明白前台、后台的概念，没有区分容器和虚拟机的差异，依旧在以传统虚拟机的角度去理解容器。

对于容器而言，其启动程序就是容器应用进程，容器就是为了主进程而存在的，主进程退出，容器就失去了存在的意义，从而退出，其它辅助进程不是它需要关心的东西。

而使用 `service nginx start` 命令，则是希望 upstart 来以后台守护进程形式启动 `nginx` 服务。而刚才说了 `CMD service nginx start` 会被理解为 `CMD [ "sh", "-c", "service nginx start"]`，因此主进程实际上是 `sh`。那么当 `service nginx start` 命令结束后，`sh` 也就结束了，`sh` 作为主进程退出了，自然就会令容器退出。

正确的做法是直接执行 `nginx` 可执行文件，并且要求以前台形式运行。比如：

```dockerfile
CMD ["nginx", "-g", "daemon off;"]
```

## ENTRYPOINT 入口点

CMD指令可以指定容器启动时要执行的命令，但它可以被docker run命令的参数覆盖掉。

您可以使用`--entrypoint`覆盖`ENTRYPOINT`，但这只能将二进制文件设置为*exec*（不会使用`sh -c`）。

ENTRYPOINT指令也可指定容器启动时要执行的命令。如果dockerfile中也有CMD指令，CMD中的参数会被附加到ENTRYPOINT 指令的后面。 如果这时docker run命令带了参数，这个参数会覆盖掉CMD指令的参数，并也会附加到ENTRYPOINT 指令的后面。

https://yeasy.gitbook.io/docker_practice/image/dockerfile/entrypoint

> 注：CMD指令和ENTRYPOINT指令当中可以使用JSON格式或者SHELL形式书写内容，但是CMD指令结合ENTRYPOINT指令使用必须是JSON格式，不然是生效的。

## **ENV 设置环境变量**

格式有两种：

- `ENV <key> <value>`
- `ENV <key1>=<value1> <key2>=<value2>...`

这个指令很简单，就是设置环境变量而已，无论是后面的其它指令，如 `RUN`，还是运行时的应用，都可以直接使用这里定义的环境变量。

```dockerfile
ENV VERSION=1.0 DEBUG=on \
    NAME="Happy Feet"
```

这个例子中演示了如何换行，以及对含有空格的值用双引号括起来的办法，这和 Shell 下的行为是一致的。

定义了环境变量，那么在后续的指令中，就可以使用这个环境变量。比如在官方 `node` 镜像 `Dockerfile` 中，就有类似这样的代码：

```dockerfile
ENV NODE_VERSION 7.2.0

RUN curl -SLO "https://nodejs.org/dist/v$NODE_VERSION/node-v$NODE_VERSION-linux-x64.tar.xz" \
  && curl -SLO "https://nodejs.org/dist/v$NODE_VERSION/SHASUMS256.txt.asc" \
  && gpg --batch --decrypt --output SHASUMS256.txt SHASUMS256.txt.asc \
  && grep " node-v$NODE_VERSION-linux-x64.tar.xz\$" SHASUMS256.txt | sha256sum -c - \
  && tar -xJf "node-v$NODE_VERSION-linux-x64.tar.xz" -C /usr/local --strip-components=1 \
  && rm "node-v$NODE_VERSION-linux-x64.tar.xz" SHASUMS256.txt.asc SHASUMS256.txt \
  && ln -s /usr/local/bin/node /usr/local/bin/nodejs
```

在这里先定义了环境变量 `NODE_VERSION`，其后的 `RUN` 这层里，多次使用 `$NODE_VERSION` 来进行操作定制。可以看到，将来升级镜像构建版本的时候，只需要更新 `7.2.0` 即可，`Dockerfile` 构建维护变得更轻松了。

下列指令可以支持环境变量展开： `ADD`、`COPY`、`ENV`、`EXPOSE`、`FROM`、`LABEL`、`USER`、`WORKDIR`、`VOLUME`、`STOPSIGNAL`、`ONBUILD`、`RUN`。

## **ARG 构建参数**

格式：`ARG <参数名>[=<默认值>]`

构建参数和 `ENV` 的效果一样，都是设置环境变量。所不同的是，`ARG` 所设置的是构建环境的环境变量，在将来容器运行时是不会存在这些环境变量的。但是不要因此就使用 `ARG` 保存密码之类的信息，因为 `docker history` 还是可以看到所有值的。

`Dockerfile` 中的 `ARG` 指令是定义参数名称，以及定义其默认值。该默认值可以在构建命令 `docker build` 中用 `--build-arg <参数名>=<值>` 来覆盖。

灵活的使用 `ARG` 指令，能够在不修改 Dockerfile 的情况下，构建出不同的镜像。

ARG 指令有生效范围，如果在 `FROM` 指令之前指定，那么只能用于 `FROM` 指令中。

```dockerfile
ARG DOCKER_USERNAME=library

FROM ${DOCKER_USERNAME}/alpine

RUN set -x ; echo ${DOCKER_USERNAME}
```

使用上述 Dockerfile 会发现无法输出 `${DOCKER_USERNAME}` 变量的值，要想正常输出，你必须在 `FROM` 之后再次指定 `ARG`

```dockerfile
# 只在 FROM 中生效
ARG DOCKER_USERNAME=library

FROM ${DOCKER_USERNAME}/alpine

# 要想在 FROM 之后使用，必须再次指定
ARG DOCKER_USERNAME=library

RUN set -x ; echo ${DOCKER_USERNAME}
```

对于多阶段构建，尤其要注意这个问题

```dockerfile
# 这个变量在每个 FROM 中都生效
ARG DOCKER_USERNAME=library

FROM ${DOCKER_USERNAME}/alpine

RUN set -x ; echo 1

FROM ${DOCKER_USERNAME}/alpine

RUN set -x ; echo 2
```

对于上述 Dockerfile 两个 `FROM` 指令都可以使用 `${DOCKER_USERNAME}`，对于在各个阶段中使用的变量都必须在每个阶段分别指定：

```dockerfile
ARG DOCKER_USERNAME=library

FROM ${DOCKER_USERNAME}/alpine

# 在FROM 之后使用变量，必须在每个阶段分别指定
ARG DOCKER_USERNAME=library

RUN set -x ; echo ${DOCKER_USERNAME}

FROM ${DOCKER_USERNAME}/alpine

# 在FROM 之后使用变量，必须在每个阶段分别指定
ARG DOCKER_USERNAME=library

RUN set -x ; echo ${DOCKER_USERNAME}
```

## **VOLUME 定义匿名卷**

格式为：

- `VOLUME ["<路径1>", "<路径2>"...]`
- `VOLUME <路径>`

之前我们说过，容器运行时应该尽量保持容器存储层不发生写操作，对于数据库类需要保存动态数据的应用，其数据库文件应该保存于卷(volume)中，后面的章节我们会进一步介绍 Docker 卷的概念。为了防止运行时用户忘记将动态文件所保存目录挂载为卷，在 `Dockerfile` 中，我们可以事先指定某些目录挂载为匿名卷，这样在运行时如果用户不指定挂载，其应用也可以正常运行，不会向容器存储层写入大量数据。

```dockerfile
VOLUME /data
```

这里的 `/data` 目录就会在容器运行时自动挂载为匿名卷，任何向 `/data` 中写入的信息都不会记录进容器存储层，从而保证了容器存储层的无状态化。当然，运行容器时可以覆盖这个挂载设置。比如：

```dockerfile
$ docker run -d -v mydata:/data xxxx
```

在这行命令中，就使用了 `mydata` 这个命名卷挂载到了 `/data` 这个位置，替代了 `Dockerfile` 中定义的匿名卷的挂载配置。

## **EXPOSE 暴露端口**

格式为 `EXPOSE <端口1> [<端口2>...]`。

`EXPOSE` 指令是声明容器运行时提供服务的端口，这只是一个声明，在容器运行时并不会因为这个声明应用就会开启这个端口的服务。在 Dockerfile 中写入这样的声明有两个好处，一个是帮助镜像使用者理解这个镜像服务的守护端口，以方便配置映射；另一个用处则是在运行时使用随机端口映射时，也就是 `docker run -P` 时，会自动随机映射 `EXPOSE` 的端口。

要将 `EXPOSE` 和在运行时使用 `-p <宿主端口>:<容器端口>` 区分开来。`-p`，是映射宿主端口和容器端口，换句话说，就是将容器的对应端口服务公开给外界访问，而 `EXPOSE` 仅仅是声明容器打算使用什么端口而已，**并不会自动在宿主进行端口映射**。

## WORKDIR 指定工作目录

格式为 `WORKDIR <工作目录路径>`。

使用 `WORKDIR` 指令可以来指定工作目录（或者称为当前目录），以后各层的当前目录就被改为指定的目录，如该目录不存在，`WORKDIR` 会帮你建立目录。

```dockerfile
WORKDIR /app

RUN echo "hello" > world.txt
```

会产生`/app/world.txt`。

如果你的 `WORKDIR` 指令使用的相对路径，那么所切换的路径与之前的 `WORKDIR` 有关：

```dockerfile
WORKDIR /a
WORKDIR b
WORKDIR c

RUN pwd
```

`RUN pwd` 的工作目录为 `/a/b/c`。

## **USER 指定当前用户**

格式：`USER <用户名>[:<用户组>]`

`USER` 指令和 `WORKDIR` 相似，都是改变环境状态并影响以后的层。`WORKDIR` 是改变工作目录，`USER` 则是改变之后层的执行 `RUN`, `CMD` 以及 `ENTRYPOINT` 这类命令的身份。

注意，`USER` 只是帮助你切换到指定用户而已，这个用户必须是事先建立好的，否则无法切换。

```dockerfile
RUN groupadd -r redis && useradd -r -g redis redis
USER redis
RUN [ "redis-server" ]
```

如果以 `root` 执行的脚本，在执行期间希望改变身份，比如希望以某个已经建立好的用户来运行某个服务进程，不要使用 `su` 或者 `sudo`，这些都需要比较麻烦的配置，而且在 TTY 缺失的环境下经常出错。建议使用 [`gosu`](https://github.com/tianon/gosu)。

```dockerfile
# 建立 redis 用户，并使用 gosu 换另一个用户执行命令
RUN groupadd -r redis && useradd -r -g redis redis
# 下载 gosu
RUN wget -O /usr/local/bin/gosu "https://github.com/tianon/gosu/releases/download/1.12/gosu-amd64" \
    && chmod +x /usr/local/bin/gosu \
    && gosu nobody true
# 设置 CMD，并以另外的用户执行
CMD [ "exec", "gosu", "redis", "redis-server" ]
```

https://blog.csdn.net/boling_cavalry/article/details/93380447

---

# 多阶段构建

- 只有`RUN`, `COPY`,`ADD`创建图层。其他指令创建临时中间图像，并且不增加构建的大小。
- 在可能的情况下，使用多阶段构建，并且只将您需要的工件复制到最终图像中。这允许您在中间构建阶段包含工具和调试信息，而不会增加最终映像的大小。

### 可以使用脚本语言进行多阶段构建

**`Dockerfile.build`**：

```dockerfile
# syntax=docker/dockerfile:1
FROM golang:1.16
WORKDIR /go/src/github.com/alexellis/href-counter/
COPY app.go ./
RUN go get -d -v golang.org/x/net/html \
  && CGO_ENABLED=0 GOOS=linux go build -a -installsuffix cgo -o app .
```

**`Dockerfile`**：

```dockerfile
# syntax=docker/dockerfile:1
FROM alpine:latest  
RUN apk --no-cache add ca-certificates
WORKDIR /root/
COPY app ./
CMD ["./app"]  
```

**`build.sh`**：

```shell
#!/bin/sh
echo Building alexellis2/href-counter:build

docker build --build-arg https_proxy=$https_proxy --build-arg http_proxy=$http_proxy \  
    -t alexellis2/href-counter:build . -f Dockerfile.build

docker container create --name extract alexellis2/href-counter:build  
docker container cp extract:/go/src/github.com/alexellis/href-counter/app ./app  
docker container rm -f extract

echo Building alexellis2/href-counter:latest

docker build --no-cache -t alexellis2/href-counter:latest .
rm ./app
```

### 使用多阶段构建

镜像的构建在构建过程的最后**阶段**。

利用构建缓存来防止重复生成已有映像

**`Dockerfile`**：

```dockerfile
# syntax=docker/dockerfile:1
FROM golang:latest
WORKDIR /go/src/github.com/alexellis/href-counter/
RUN go get -d -v golang.org/x/net/html  
COPY app.go ./
RUN CGO_ENABLED=0 GOOS=linux go build -a -installsuffix cgo -o app .

FROM alpine:latest  
RUN apk --no-cache add ca-certificates
WORKDIR /root/
COPY --from=0 /go/src/github.com/alexellis/href-counter/app ./
CMD ["./app"]  
```

其中`COPY --from=0`将从阶段0中复制映像到当前阶段并使用，而不用重新建立新的镜像

### 只构建某一阶段

我们可以使用 `as` 来为某一阶段命名，例如

```dockerfile
FROM golang:1.16 as builder
```

例如当我们只想构建 `builder` 阶段的镜像时，增加 `--target=builder` 参数即可

```bash
$ docker build --target builder -t username/imagename:tag .
```

### 构建时从其他镜像复制文件

上面例子中我们使用 `COPY --from=0 /go/src/github.com/alexellis/href-counter/app ./`从上一阶段的镜像中复制文件，我们也可以复制任意镜像中的文件。

```dockerfile
COPY --from=nginx:latest /etc/nginx/nginx.conf /nginx.conf
```

### 使用前一个阶段作为新阶段

使用`FROM`指令时，可以通过引用上一阶段停止的位置来继续。例如：

```dockerfile
# syntax=docker/dockerfile:1
FROM alpine:latest AS builder
RUN apk --no-cache add build-base

FROM builder AS build1
COPY source1.cpp source.cpp
RUN g++ -o /binary source.cpp

FROM builder AS build2
COPY source2.cpp source.cpp
RUN g++ -o /binary source.cpp
```

