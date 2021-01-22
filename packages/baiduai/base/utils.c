int http_percentage_coding(unsigned char *org_data, unsigned char *new_data, int len)
{
    int i = 0;
    unsigned char org_char = 0;

    while (len--)
    {
        org_char = *(org_data++);
        switch (org_char)
        {
        case ':' :
            new_data[i++] = '%';
            new_data[i++] = '3';
            new_data[i++] = 'A';
        break;

        case '/' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = 'F';
        break;

        case '?' :
            new_data[i++] = '%';
            new_data[i++] = '3';
            new_data[i++] = 'F';
        break;

        case '#' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '3';
        break;

        case '[' :
            new_data[i++] = '%';
            new_data[i++] = '5';
            new_data[i++] = 'B';
        break;

        case ']' :
            new_data[i++] = '%';
            new_data[i++] = '5';
            new_data[i++] = 'D';
        break;

        case '@' :
            new_data[i++] = '%';
            new_data[i++] = '4';
            new_data[i++] = '0';
        break;

        case '!' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '1';
        break;

        case '$' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '4';
        break;

        case '&' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '6';
        break;

        case '\'' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '7';
        break;

        case '(' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '8';
        break;

        case ')' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '9';
        break;

        case '*' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = 'A';
        break;

        case '+' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = 'B';
        break;

        case ',' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = 'C';
        break;

        case ';' :
            new_data[i++] = '%';
            new_data[i++] = '3';
            new_data[i++] = 'B';
        break;

        case '=' :
            new_data[i++] = '%';
            new_data[i++] = '3';
            new_data[i++] = 'D';
        break;

        case '%' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '5';
        break;

        case ' ' :
            new_data[i++] = '%';
            new_data[i++] = '2';
            new_data[i++] = '0';
        break;

        default:
            new_data[i++] = org_char;
        break;
        }
    }
    return i;
}
