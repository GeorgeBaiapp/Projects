# This is me learning how to use R
# Through a tutorial

library(datasets)
head(iris)

?plot

plot(iris$Species)
plot(iris$Petal.Length)
plot(iris$Species, iris$Petal.Width)
plot(iris$Petal.Length, iris$Petal.Width)
plot(iris)

plot(iris$Petal.Length, iris$Petal.Width,
     col = "#cc0000",
     pch = 19,
     main = "Iris: Petal Length vs Petal Width",
     xlab = "Petal Length",
     ylab = "Petal Width"
     )

plot(cos, 0, 2*pi)
plot(exp, 1, 5)
plot(dnorm, -3, +3)

plot(dnorm, -3, +3,
     col = "#cc0000",
     lwd=5,
     main = "Standard Normal Distribution",
     xlab = "Z-scores",
     ylab = "Density"
     )

?mtcars
head(mtcars)

barplot(mtcars$cyl)
cylinders <- table(mtcars$cyl)
barplot(cylinders)
plot(cylinders)

hist(iris$Sepal.Length)
hist(iris$Petal.Length)

par(mfrow = c(3, 1))
hist(iris$Petal.Width [iris$Species == "setosa"],
     xlim = c(0, 3),
     breaks = 9,
     main = "Petal Width for Setosa",
     xlab = "",
     col = "red"
     )

hist(iris$Petal.Width [iris$Species == "versicolor"],
     xlim = c(0, 3),
     breaks = 9,
     main = "Petal Width for Versicolor",
     xlab = "",
     col = "purple"
)

hist(iris$Petal.Width [iris$Species == "virginica"],
     xlim = c(0, 3),
     breaks = 9,
     main = "Petal Width for virginica",
     xlab = "",
     col = "blue"
)

par(mfrow = c(1, 1))

hist(mtcars$wt)
hist(mtcars$mpg)

plot(mtcars$wt, mtcars$mpg,
     pch = 19,
     cex = 1.5,
     col = "#cc0000",
     main = "MPG as a Function of Weight on Cars",
     xlab = "Weight (in 1000 pounds)",
     ylab = "MPG")
     )

i.setosa <- iris[iris$Species == "setosa",]
head(i.setosa)
summary(i.setosa$Petal.Length)
hist(i.setosa$Petal.Length)

n1 <- 15
n1
typeof(n1)

n2 <- 1.5
n2
typeof(n1)

c1 <- "c"
c1
typeof(c1)

c2 <- "a string of text"
c2
typeof(c2)

l1 <- TRUE
l1
typeof(l1)

l2 <- F
l2
typeof(l2)

v1 <- c(1, 2, 3, 4, 5)
v1
is.vector(v1)

v2 <- c("a", "b", "c")
v2
is.vector(v2)

v3 <- c(TRUE, TRUE, FALSE, FALSE, TRUE)
v3
is.vector(v3)

m1 <- matrix(c(T, T, F, F, T, F), nrow=2)
m1

m2 <- matrix(c("a", "b", "c", "d"), nrow=2, byrow=T)
m2

# 4 rows, 3 columns, 2 sheets
a1 <- array(c(1:24), c(4, 3, 2))
a1

vNumeric <- c(1, 2, 3)
vCharacter <- c("a", "b", "c")
vLogical <- c(T, F, T)

dfa <- cbind(vNumeric, vCharacter, vLogical)
dfa

df <- as.data.frame(cbind(vNumeric, vCharacter, vLogical))
df

list1 <- list(v1, v2, v3)
list1

list2 <- list(v1, v2, v3, list1)
list2

(corce1 <- c(1, "b", TRUE))
coerce2 <- 5

coerce3 <- as.integer(5)
typeof(coerce3)

coerce5 <- as.numeric(c("1", "2"))
typeof(coerce5)

(coerce7 <- as.data.frame(matrix(1:9, nrow=3)))

(x1 <- 1:3)
(y <- 1:9)

(df1 <- cbind.data.frame(x1, y))
typeof(df1$x1)

(x2 <- as.factor(c(1:3)))
(df2 <- cbind.data.frame(x2, y))
