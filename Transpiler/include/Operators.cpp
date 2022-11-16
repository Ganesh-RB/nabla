#include "Operators.h"

Transpose::Transpose(Node* a, int count){
    trans_count = count;
    inputs.push_back(a);
    this->name = "Transpose: " + std::to_string(trans_count);
    this->forward(a);
}

Node* Transpose::forward(const Node* a){
    this->data = inputs[0]->data.transpose();
    return this;
}

void Transpose::backward(){
    inputs[0]->gradient = add(inputs[0]->gradient, this->gradient.transpose());
}

Negative::Negative(Node* a, int count){
    neg_count = count;
    inputs.push_back(a);
    this->name = "Negative: " + std::to_string(neg_count);
    this->forward(a);
}

Node* Negative::forward(const Node* a){
    this->data = inputs[0]->data.negative();
    return this;
}

void Negative::backward(){
    inputs[0]->gradient = add(inputs[0]->gradient, this->gradient.negative());
}

Sub::Sub(Node* a, Node* b, int count){
    sub_count = count;
    inputs.push_back(a);
    inputs.push_back(b);
    this->name = "Sub:" + std::to_string(sub_count);
    this->forward(a, b);
}

Node* Sub::forward(const Node* a, const Node* b){
    if(a->is_scalar && b->is_scalar){
        this->ddata = a->ddata - b->ddata;
        this->is_scalar = true; //you are a scalar now!
        return this; //return yourself 
    }
    Tensor* c = new Tensor(a->data.m, a->data.n);
    for(int i=0;i<a->data.m;i++){
        for(int j=0;j<a->data.n;j++){
            c->data[i][j] = a->data.data[i][j] - b->data.data[i][j];
        }
    } //this gradient should be zero and also initialized
    // Node* n_c = new Node(*c);
    this->data = *c;
    this->gradient = Tensor(c->m, c->n); //#!!!!!!! I am initializing my gradient tensor to all zeros ? is this fine!!?
    return this;
}

void Sub::backward()
{
    if(this->is_scalar){
            inputs[0]->scalar_gradient += this->scalar_gradient;
            inputs[1]->scalar_gradient -= this->scalar_gradient;
            return;
    }
    inputs[0]->gradient = add(inputs[0]->gradient , this->gradient) ; //should be an addition to the gradient flow,, not a copy right?
    inputs[1]->gradient = add(inputs[1]->gradient , this->gradient.negative()) ; //should be an addition to the gradient flow,, not a copy right?
}

Add::Add(Node* a, Node* b, int count){
    // ::count++;
    add_count = count;
    inputs.push_back(a);
    inputs.push_back(b);
    this->name = "Add:" + std::to_string(add_count);
    this->forward(a, b); //the construction itself will do the forward pass
}

Node* Add::forward(const Node* a, const Node* b)
{
    if(a->is_scalar && b->is_scalar){
        this->ddata = a->ddata + b->ddata;
        this->is_scalar = true; //you are a scalar now!
        return this; //return yourself 
    }
    Tensor* c = new Tensor(a->data.m, a->data.n);
    for(int i=0;i<a->data.m;i++){
        for(int j=0;j<a->data.n;j++){
            c->data[i][j] = a->data.data[i][j] + b->data.data[i][j];
        }
    } //this gradient should be zero and also initialized
    // Node* n_c = new Node(*c);
    this->data = *c;
    this->gradient = Tensor(c->m, c->n); //#!!!!!!! I am initializing my gradient tensor to all zeros ? is this fine!!?
    return this;
}

void Add::backward()
{
    if(this->is_scalar){
        for (auto &x: inputs){
            x->scalar_gradient += this->scalar_gradient;
        }
        return;
    }
    for(auto& x : inputs){
        x->gradient = add(x->gradient , this->gradient) ; //should be an addition to the gradient flow,, not a copy right?
    }
}


Multiply::Multiply(Node* a , Node* b , int count){
        mul_count = count;
        inputs.push_back(a);
        inputs.push_back(b);
        this->name = "Mat_Mul:" + std::to_string(mul_count);
        this->forward(a, b);
}

Node* Multiply :: forward(const Node* a, const Node* b)
{
    //this is an operation only defined between tensors!, so no need to check for scalars
    Tensor* c = new Tensor(a->data.m, b->data.n);
    for (int i = 0; i< a->data.m ; i++){
        for (int j = 0; j < b->data.n; j++){
            (c->data)[i][j] = 0;
            for (int k = 0; k < a->data.n; k++){
                (c->data)[i][j] += (a->data.data)[i][k] * (b->data.data)[k][j];
            }
        }
    }
    this->data = *c;
    this->gradient = Tensor(c->m, c->n); //initialze the gradeint tensor to all zeroes, is this necessary?
    return this;
}

void Multiply :: backward(){
    this->inputs[0]->gradient = matmul(this->gradient, inputs[1]->data.transpose());
    this->inputs[1]->gradient = matmul(inputs[0]->data.transpose(), this->gradient);
    // std::cout<<"Setting gradients for matmul op"<<std::endl;
}

Mul::Mul(Node* a , Node* b , int count){
        mul_count = count;
        inputs.push_back(a);
        inputs.push_back(b);
        this->name = "Mul:" + std::to_string(mul_count);
        this->forward(a, b);
}

Node* Mul::forward(const Node* a , const Node* b){
    if(a->is_scalar && b->is_scalar){
        this->ddata = a->ddata * b->ddata;
        this->is_scalar = true;
        return this;
    }
    if(a->is_scalar && !b->is_scalar){
        Tensor* c = new Tensor(b->data.m, b->data.n);
        for(int i=0;i<b->data.m;i++){
            for(int j=0;j<b->data.n;j++){
                c->data[i][j] = a->ddata * b->data.data[i][j];
            }
        }
        this->data = *c;
        this->gradient = Tensor(c->m, c->n);
        return this;
    }
    if(!a->is_scalar && b->is_scalar){
        Tensor* c = new Tensor(a->data.m, a->data.n);
        for(int i=0;i<a->data.m;i++){
            for(int j=0;j<a->data.n;j++){
                c->data[i][j] = a->data.data[i][j] * b->ddata;
            }
        }
        this->data = *c;
        this->gradient = Tensor(c->m, c->n);
        return this;
    }

    Tensor* c = new Tensor(a->data.m, a->data.n);
    for(int i=0;i<a->data.m;i++){
        for(int j=0;j<a->data.n;j++){
            c->data[i][j] = a->data.data[i][j] * b->data.data[i][j];
        }
    }
    this->data = *c;
    this->gradient = Tensor(c->m, c->n);
    return this;
}

void Mul::backward(){
    if(this->is_scalar){
        this->inputs[0]->scalar_gradient += this->scalar_gradient * this->inputs[1]->ddata;
        this->inputs[1]->scalar_gradient += this->scalar_gradient * this->inputs[0]->ddata;
    }
    else{
        if(this->inputs[0]->is_scalar){
            //to add the tensor element wise multiplications
            //dl/db = a*G //scalar multiplication
            //dl/da  = sum(G elementwsie M)//note that you can access all the forward values automatically through node !
            this->inputs[1]->gradient = add(this->inputs[1]->gradient , mul(this->inputs[0]->ddata,  this->gradient));
            this->inputs[0]->scalar_gradient = (this->inputs[0]->scalar_gradient  + full_sum(mul(this->gradient, this->inputs[1]->data)));
        }
        else if(this->inputs[1]->is_scalar){
            this->inputs[0]->gradient = add(this->inputs[0]->gradient , mul(this->inputs[1]->ddata,  this->gradient));
            this->inputs[1]->scalar_gradient = (this->inputs[1]->scalar_gradient  + full_sum(mul(this->gradient, this->inputs[0]->data)));
        }
        else{
            this->inputs[0]->gradient = add(this->inputs[0]->gradient , mul(this->inputs[1]->data,  this->gradient));
            this->inputs[1]->gradient = add(this->inputs[1]->gradient , mul(this->inputs[0]->data,  this->gradient));
        }
    }
}
