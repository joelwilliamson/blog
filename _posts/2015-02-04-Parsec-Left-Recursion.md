---
layout:	post
title:	Parsec and Left Recursive Grammars
categories: [programming, parsing, haskell]
---
The traditional way to specify the syntax of a programming language is to write
a Context Free Grammar (CFG) in Extended Backus-Naur Form (EBNF). Briefly, this
form consists of a series of rules, each describing the form of some syntax
element. For example, we might have a rule for postfix expression in C:

    postfix-expression:
       primary-expression
       | postfix-expression '[' expression ']'
       | postfix-expression '(' argument-expression-list ')'
       | postfix-expression '.' identifier
       ⋮

This says that a postfix expression can be a primary expression (constant,
identifier, or expression in parentheses) or it can be a postfix expression
followed by some operator. An expression like

{% highlight c %}
rectangle.bottomLeft.x
{% endhighlight %}

is thus parsed as

    \{\{primary-expression\} '.' identifier\} '.' identifier

The rule for `postfix-expression` is known as **left recursive** because
`postfix-expression` appear as the leftmost symbol in some of the productions.
This causes a problem for most left-to-right parser. When they attempt to parse
some a postfix expression, they might first attempt to parse it as a
`primary-expression`. If that fails, they might attempt to parse it as an array
subscript. To do this, they first need to parse a `postfix-expression`. This
leads to an infinite recursion, where the parser continues to nest deeper and
deeper, while never consuming any input.

The traditional solution is to rewrite our rule into a **right recursive** form.
In this case, we notice that every `postfix-expression` begins with a
`primary-expression`, followed by a (possibly empty) chain of subscripts, member accesses and
function calls.
We can thus rewrite the grammar as

    postfix-expression: primary-expression postfix-expression'

    postfix-expression': empty
                       | '[' expression ']' postfix-expression'
		       | '(' argument-expression-list ')' postfix-expression'
		       | '.' identifier postfix-expression'
		       ⋮

Unfortunately, this changes the associativity of the operations. Instead of
being left associative, they are now right associative, and our example would be
parsed as

    primary-expression \{'.' identifier \{'.' identifier\}\}

The solution to this is somewhat dependent on the particulars of your parser.
I have been using [Parsec](https://wiki.haskell.org/Parsec) for parsing. The
left recursive grammar would be written as

{% highlight haskell %}
data PostfixExpression = Primary PrimaryExpression
     		       | ArraySubscript PostfixExpression Expression
		       | FunctionCall PostfixExpression [ArgumentExpression]
		       | Member PostfixExpression Identifier
		       ⋮
		       
postfixExpression =
    (try $ liftM2 ArraySubscript postfixExpression $ brackets expression)
    <|> (try $ liftM2 FunctionCall postfixExpressioon $ parens $ many argumentExpression)
    <|> (try $ liftM2 Member postfixExpression (dot >> identifier))
    ⋮
    <|> liftM Primary primaryExpression
{% endhighlight %}

This builds an abstract syntax tree and nicely matches the structure of the
grammar. Unfortunately, it doesn't complete (as we have seen). The advice on most
blogs I have seen for dealing with left recursion is to use the `chainl1`
combinator. This would be a nice solution, but it assumes all the subexpressions
have the same type as the result of combining them. `chainl1` can be adapted to
handle the case of subexpressions of type `a` and a result of type `b`. This was
useful for handling arithmetic expressions.

{% highlight haskell %}
chainl1' :: Parser a -> Parser (b -> a -> b) -> (a -> b) -> Parser b
chainl1' parser op baseConstructor = parser >>= (rest . baseConstructor)
    where rest context = do
			     f <- op
			     y <- parser
			     rest $ f context y
			 <|> return context

data AdditiveExpression = MultiplicativeExpression MultiplicativeExpression
     			| Add AdditiveExpression MultiplicativeExpression
			| Subtract AdditiveExpression MultiplicativeExpression

parsePlusMinus = (reservedOp '+' >> return Add) <|> (reservedOp '-' >> return Subtract)
additiveExpression = chainl1' multiplicativeExpression parsePlusMinus MultiplicativeExpression
{% endhighlight %}

This works beautifully for defining the arithmetic of a language, which is often
many levels deep and each level has similar structure to all the others. Postfix
expressions lack this regular structure, so we need a slightly more involved
approach. It might be possible to make an even more general chain combinator,
which associates each operator parser with an expression parser, but it would
only be used in this location, so it is simpler to just handle this case
independently. The structure is similar to chainl1', but with several
alternatives

{% highlight haskell %}
postfixExpression = rest $ liftM Primary primaryExpression
    where rest context = (rest $ liftM2 ArraySubscript context $ brackets expression)
    	       	       	 <|> (rest $ liftM2 FunctionCall context $ parens expression)
			 <|> (rest $ liftM2 Member context $ dot >> identifier)
			 <|> context
{% endhighlight %}

This runs nicely and still preserves clarity.

Coming up with this solution made showed just how valuable having the source for
a solution that does something similar to what you want is, and how valuable
having a package management system (Hackage) that can show source is.