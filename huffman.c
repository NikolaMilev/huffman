#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// character comparison, for qsort
int cmp_chars(const void* first, const void* second)
{
	char f = *(char*)first;
	char s = *(char*)second;

	return f-s;
}

// left and right structure fields contain NULL iff the node is 
// a leaf
typedef struct node{
	unsigned cnt;
	char* ch;
	struct node* left;
	struct node* right;
} node;

// makd node from the data given as arguments
node* make_node(unsigned cnt, char* ch, node* left, node* right)
{
	node* n = (node*)malloc(sizeof(node));
	if(!n)
	{
		perror("malloc: ");
		exit(EXIT_FAILURE);
	}

	n->left = left;
	n->right = right;
	n->ch = ch;
	n->cnt = cnt;

	return n;
}

// sort all the characters so that all of the same characters
// are grouped together; this is fine because the frequency is
// important but the position of the characters is not
char* sort_chars(const char* s)
{
	char *string;
	string = (char*)malloc(strlen(s)+1);
	if(!string)
	{
		printf("4\n");
		perror("malloc: ");
		exit(EXIT_FAILURE);
	}
	strcpy(string, s);
	qsort(string, strlen(string), sizeof(char), cmp_chars);
	return string;
}

// count all the different symbols in the string, making a node* array
node** count_all(const char* s, int* len)
{
	// sort the string, qsort
	
	char *string = sort_chars(s);
	
	node** arr;
	
	// count each symbol
	arr = (node**)malloc(sizeof(node*)*strlen(string));
	if(!arr)
	{
		printf("3\n");
		perror("malloc: ");
		exit(EXIT_FAILURE);
	}

	int i = -1, cnt = 0;
	char* c = string;
	char prev = -1;
	//printf("\n\"%s\"\n %d \n", string, strlen(string));

	while(*c)
	{	
		// we assume that the string has already been sorted
		// with that assumption, every time we find a new character, we make
		// a new node
		cnt++;
		if(*c != prev)
		{
			char *tmp_ch = (char*)malloc(2);
			tmp_ch[0] = *c;
			tmp_ch[1] = 0;
			arr[++i] = make_node(1, tmp_ch, NULL, NULL);
			prev = *c;
			c++;	
		}
		else // otherwise, the symbol is the "old one" so we just
			// increment the counter
		{
			arr[i]->cnt++;
			c++;
		}
		
	}
	
	*len = i+1;	
	return arr;
}

// compare two nodes by character frequency
int cmp_node_r(const void* first, const void* second)
{
	node* f = *(node**)first;
	node* s = *(node**)second;

	return (f->cnt)-(s->cnt);
}

//now we make a tree
node* huffman_tree(node** arr, int len)
{
	// find the least frequent (the beginning of the array)
	// merge them into one node and insert into the array, 
	// with frequency in mind so that the array stays sorted

	// printf("Length: %d\n", len);
	// for(i=0; i<len; i++)
	// {
	// 	printf("%s %u\t", arr[i]->ch, arr[i]->cnt);
	// }
	qsort(arr, len, sizeof(node*), cmp_node_r);
	// printf("\n");
	// for(i=0; i<len; i++)
	// {
	// 	printf("%s %u\t", arr[i]->ch, arr[i]->cnt);
	// }
	// printf("\n--------------------------------------------------\n");
	if(len < 1)
	{
		fprintf(stderr, "We can't apply the algorithm for less than 1 element!\n");
		exit(EXIT_FAILURE);
	}
	if(len == 1)
	{
		return arr[0];
	}
	else
	{
		char* ch = (char*)malloc(strlen(arr[0]->ch) + strlen(arr[1]->ch));
		strcpy(ch, arr[0]->ch);
		strcat(ch, arr[1]->ch);
		arr[1] = make_node(arr[0]->cnt + arr[1]->cnt, ch, arr[0], arr[1]);;
		
		return huffman_tree(arr+1, len-1);
	}

}

void print_tree(node* drvo)
{
	if(drvo)
	{
		printf("%s %d\n", drvo->ch, drvo->cnt);
		print_tree(drvo->left);
		print_tree(drvo->right);
	}
}

char* string_reverse(const char* string)
{
	char* s = (char*)malloc(strlen(string)+1);
	int i;
	if(!s)
	{
		printf("7\n");
		perror("malloc: ");
		exit(EXIT_FAILURE);
	}

	s[strlen(string)] = 0;
	for(i = 0; i < strlen(string); i++)
	{
		s[i] = string[strlen(string) - i - 1];
	}

	return s;
}

void print_codes(node* drvo, const char* prefix, char* codes[], char characters[], int freqs[], int* index)
{
	if(strlen(drvo->ch) == 1)
	{
		codes[*index] = string_reverse(prefix);
		characters[*index] = drvo->ch[0];
		freqs[*index] = drvo->cnt;
		(*index)++;
		//printf("%s : %s\n", drvo->ch, spom);
		return;
	}

	char* s = malloc(strlen(prefix) + 2);
	if(!s)
	{
		printf("6\n");
		perror("malloc: ");
		exit(EXIT_FAILURE);
	}

	

	s[0] = '0';
	s[1] = 0;
	strcat(s, prefix);
	print_codes(drvo->left, s, codes, characters, freqs, index);

	s[0] = '1';
	s[1] = 0;
	strcat(s, prefix);
	print_codes(drvo->right, s, codes, characters, freqs, index);

	free(s);
}


// nesto nije ok u kreiranju drveta, los poredak
// proveri sortiranje
// qsort ne prima lepo


int main(int argc, char* argv[])
{
	node **forest;
	node* huffman;
	int len, i;
	char* codes[100];
	char characters[100];
	int freqs[100];
	int index = 0;
	char* text = argv[1];
	forest = count_all(text, &len);

	int bit_num = (int)ceil(log(len)/log(2));
	int bit_sum = strlen(text)*bit_num;

	printf("We need %d bits without Huffman compression!\n", bit_sum);

	// for(i = 0; i < len; i++)
	// {
	// 	node *n = forest[i];
	// 	printf("%c %u\n", *n->ch, n->cnt);
	// }

	huffman = huffman_tree(forest, len);
	print_codes(huffman, "", codes, characters, freqs, &index);
	//print_tree(huffman);
	printf("Codes for each symbol:\n");
	bit_sum = 0;
	for(i = 0; i < index; i++)
	{	
		bit_sum += freqs[i]*strlen(codes[i]);
		printf("%c %s\n", characters[i], codes[i]);

	}

	printf("We need %d bits with Huffman compression!\n", bit_sum);

	return 0;
}