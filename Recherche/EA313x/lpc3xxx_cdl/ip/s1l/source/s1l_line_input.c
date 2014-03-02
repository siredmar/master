/***********************************************************************
 * $Id:: s1l_line_input.c 3429 2010-05-07 17:29:59Z usb10132           $
 *
 * Project: Line input method and prompt controls
 *
 * Description:
 *     Used to input commands and arguments from an input method
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only  
 * which provides customers with programming information regarding the  
 * products. This software is supplied "AS IS" without any warranties.  
 * NXP Semiconductors assumes no responsibility or liability for the 
 * use of the software, conveys no license or title under any patent, 
 * copyright, or mask work right to the product. NXP Semiconductors 
 * reserves the right to make changes in the software without 
 * notification. NXP Semiconductors also make no representation or 
 * warranty that such application will be suitable for the specified 
 * use without further testing or modification. 
 **********************************************************************/

#include "lpc_string.h"
#include "s1l_sys_inf.h"
#include "s1l_line_input.h"

/* Important keys */
#define KEY_BACKSPACE 8
#define KEY_NL        10
#define KEY_CR        13
#define KEY_ESC       27
#define KEY_SPACE     32

/* Structure used for connecting key presses with specific operational
   functions */
typedef void (*PFVI)(UNS_8);
typedef struct {
	UNS_8 keymin;
	UNS_8 keymax;
	PFVI func;
} KEY_REDIRECT_T;

/* Prototypes needed for indirect call structure */
void key_backspace(UNS_8 ch);
void key_line_enter(UNS_8 ch);
void key_esc(UNS_8 ch);
void key_textin(UNS_8 ch);
void key_line_delete(void);
void key_hist_up(void);
void key_hist_down(void);

/* Escape sequences and callbacks for specific key codes */
UNS_8 key_up[] = {91, 65, 255};
UNS_8 key_down[] = {91, 66, 255};
UNS_8 key_del[] = {91, 52, 126, 255};
UNS_8 *keyescseqs[] = {key_up, key_down, key_del, NULL};
PFV keyescfuncs[] = {key_hist_up, key_hist_down, key_line_delete, NULL};
static UNS_8 yesno_msg[] = "(y/n):";

/* Supported key sequences and functions - all keys outside this range
   are not supported. */
static KEY_REDIRECT_T keysred[] = {
	{KEY_BACKSPACE, KEY_BACKSPACE, key_backspace},
	{KEY_CR, KEY_CR, key_line_enter},
	{KEY_ESC, KEY_ESC, key_esc},
	{32, 126, key_textin},
	{255, 0, NULL}
};

/* Local data */
static UNS_8 cmd_prompt[16];
static UNS_8 currcmdline[256];
static int escidx, currcmdlineindex;
static BOOL_32 cmdpending;
static BOOL_32 esckey;
static UNS_8 keyescbuff[16];
static UNS_8 delstr[3] = {KEY_BACKSPACE, KEY_SPACE, KEY_BACKSPACE};

#ifdef HISTORY_ENABLE
#define MAX_HISTORY 16
static UNS_8 str_hist[MAX_HISTORY][256];
static UNS_8 str_hist_size[MAX_HISTORY];
static int hist_index, hist_items, cur_hist_check;
#endif

/***********************************************************************
 *
 * Function: key_hist_up
 *
 * Purpose: Shows previous command in history list
 *
 * Processing:
 *     Not supported yet
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_hist_up(void) {
#ifdef HISTORY_ENABLE
	if (hist_items > 0) {
		/* Delete current line and buffer */
		key_line_delete();

		/* Decrement next check index by 1 */
		cur_hist_check--;
		if (cur_hist_check < 0)
			cur_hist_check = hist_items - 1;

		/* Copy history into buffer */
		currcmdlineindex = str_hist_size[cur_hist_check] - 1;
		str_ncopy(currcmdline, str_hist[cur_hist_check],
			str_hist_size[cur_hist_check]);

		/* Display new command line */
		term_dat_out_len(currcmdline, str_hist_size[cur_hist_check]);
	}
#endif
}

/***********************************************************************
 *
 * Function: key_hist_down
 *
 * Purpose: Shows next command in history list
 *
 * Processing:
 *     Not supported yet
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_hist_down(void) {
#ifdef HISTORY_ENABLE
	if (hist_items > 0) {
		/* Delete current line and buffer */
		key_line_delete();

		/* Increment next check index by 1 */
		cur_hist_check++;
		if (cur_hist_check >= hist_items)
			cur_hist_check = 0;

		/* Copy history into buffer */
		currcmdlineindex = str_hist_size[cur_hist_check] - 1;
		str_ncopy(currcmdline, str_hist[cur_hist_check],
			str_hist_size[cur_hist_check]);

		/* Display new command line */
		term_dat_out_len(currcmdline, str_hist_size[cur_hist_check]);
	}
#endif
}

/***********************************************************************
 *
 * Function: key_line_delete
 *
 * Purpose: Deletes the current display line
 *
 * Processing:
 *     While there is data displayed (in the command buffer), issue
 *     backspaces until it is gone.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_line_delete(void) {
	while (currcmdlineindex > 0) {
		term_dat_out_len(delstr, 3);
		currcmdlineindex--;
	}
	currcmdline[0] = '\0';
}

/***********************************************************************
 *
 * Function: key_backspace
 *
 * Purpose: Deletes the last key pressed in the command line
 *
 * Processing:
 *     If there is data in the command line, delete the last key in
 *     it.
 *
 * Parameters:
 *     ch : Input key
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_backspace(UNS_8 ch) {
	if (currcmdlineindex > 0) {
		term_dat_out_len(delstr, 3);
		currcmdlineindex--;
	}
	currcmdline[currcmdlineindex] = '\0';
}

/***********************************************************************
 *
 * Function: key_line_enter
 *
 * Purpose: Sets a command line as ready to process (pending)
 *
 * Processing:
 *     If there is data in the command line, set the command pending
 *     flag to process the data.
 *
 * Parameters:
 *     ch : Input key
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_line_enter(UNS_8 ch) {
	if (currcmdlineindex > 0) {
		cmdpending = TRUE;
		term_dat_out_crlf((UNS_8 *) "");
	}
}

/***********************************************************************
 *
 * Function: key_textin
 *
 * Purpose: Adds key data to the command line
 *
 * Processing:
 *     If the passed key will fit in the buffer, add it and place the
 *     terminator at the next index.
 *
 * Parameters:
 *     ch : Input key
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_textin(UNS_8 ch) {
	if (currcmdlineindex < 254) {
		currcmdline[currcmdlineindex] = ch;
		currcmdlineindex++;
		term_dat_out_len(&ch, 1);
		currcmdline[currcmdlineindex] = '\0';
	}
}

/***********************************************************************
 *
 * Function: key_esc_handle
 *
 * Purpose: Captures an escape sequence and handles it
 *
 * Processing:
 *     Set the scape key flag
 *
 * Parameters:
 *     ch : Input key
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_esc_handle(UNS_8 ch) {
	int idx2, idx = 0;
	UNS_8 *pcseq;
	BOOL_32 srchcomp, handled = FALSE;

	keyescbuff [escidx] = ch;
	escidx++;
	keyescbuff [escidx] = '\0';

	/* Escape sequence too long? */
	if (escidx > 8)
	{
		/* Reset escape sequence */
		esckey = FALSE;
		escidx = 0;
	}

	/* Verify against the known escape sequences */
	while ((handled == FALSE) && (keyescseqs[idx] != NULL))
	{
		idx2 = 0;
		pcseq = (UNS_8 *) keyescseqs[idx];
		srchcomp = TRUE;
		while ((srchcomp == TRUE) && (*pcseq != 255))
		{
			if (*pcseq != keyescbuff[idx2]) 
			{
				/* Not a matching escape sequence */
				srchcomp = FALSE;
			}

			idx2++;
			pcseq++;
		}

		/* Was a match found? */
		if ((*pcseq == 255) && (srchcomp == TRUE))
		{
			handled = TRUE;
			esckey = FALSE;
			keyescfuncs[idx]();
		}

		idx++;
	}
}

/***********************************************************************
 *
 * Function: key_esc
 *
 * Purpose: Starts an escape sequence
 *
 * Processing:
 *     Set the scape key flag
 *
 * Parameters:
 *     ch : Input key
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_esc(UNS_8 ch) {
	esckey = TRUE;
	escidx = 0;
}

/***********************************************************************
 *
 * Function: key_command_clear
 *
 * Purpose: Clears command line
 *
 * Processing:
 *     Clears command index and command flag.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
 void key_command_clear(void)
 {
	/* Clear current command line */
	currcmdlineindex = 0;
	cmdpending = FALSE;
	esckey = FALSE;
	currcmdline[0] = '\0';
 }

/***********************************************************************
 *
 * Function: key_prompt_set
 *
 * Purpose: Sets up prompt
 *
 * Processing:
 *     Sets the current command prompt.
 *
 * Parameters:
 *     prompt    : Prompt to display for command line
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_prompt_set(UNS_8 *prompt) 
{
	str_ncopy(cmd_prompt, prompt, 16);
}

/***********************************************************************
 *
 * Function: key_line_init
 *
 * Purpose: Initializes line input (key) methods
 *
 * Processing:
 *     Finds the string terminator. If the terminator exceeds a string
 *     length of 255 characters, the terminator is added at position
 *     256.
 *
 * Parameters:
 *     prompt    : Prompt to display for command line
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_line_init(UNS_8 *prompt)
{
	/* Save prompt */
	key_prompt_set(prompt);

#ifdef HISTORY_ENABLE
	/* Clear history */
	hist_index = 0;
	hist_items = 0;
	cur_hist_check = 0;
#endif

	/* Clear current command line */
	key_command_clear();
}

/***********************************************************************
 *
 * Function: key_handle
 *
 * Purpose: Handles key presses
 *
 * Processing:
 *    For the passed key, verify that the keypress is defined. If it
 *    is good, call the callback function for that keypress.
 *
 * Parameters:
 *     key : Key event to handle
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_handle(UNS_8 key)
{
	BOOL_32 handled = FALSE;
	int idx = 0;

	while ((keysred[idx].keymin <= keysred[idx].keymax) &&
		(handled == FALSE)) {
		if (esckey == TRUE) 
		{
			key_esc_handle(key);
			handled = TRUE;
		}
		else if ((key >= keysred[idx].keymin) &&
			(key <= keysred[idx].keymax)) {
			keysred[idx].func(key);
			handled = TRUE;
		}

		idx++;
	}
}

/***********************************************************************
 *
 * Function: key_get_command
 *
 * Purpose: Gets a command from the command prompt
 *
 * Processing:
 *     Display the command prompt
 *
 * Parameters:
 *     str : Command string to fill
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void key_get_command(UNS_8 *str)
{
	UNS_8 key;

	/* Display prompt */
	term_dat_out_crlf((UNS_8 *) "");
	term_dat_out(cmd_prompt);

	key_command_clear();
	while (cmdpending == FALSE) {
		if (term_dat_in_ready() > 0) {
			term_dat_in(&key, 1);
			key_handle((UNS_8) key);
		}
	}

	/* Copy input string to command string */
	str_ncopy(str, currcmdline, 256);

#ifdef HISTORY_ENABLE
	/* Copy the current line buffer into the history array */
	str_hist_size[hist_index] = currcmdlineindex + 1;
	str_ncopy(&str_hist[hist_index], currcmdline,
		str_hist_size[hist_index]);
	hist_index++;
	cur_hist_check = hist_index;
	if (hist_index >= MAX_HISTORY)
		hist_index = 0;

	hist_items++;
	if (hist_items >= MAX_HISTORY)
		hist_items = MAX_HISTORY;
#endif
}

/***********************************************************************
 *
 * Function: prompt_yesno
 *
 * Purpose: Displays a prompt with a yes/no input
 *
 * Processing:
 *     Display a yes/no prompt. Wait for a key to be pressed. If the
 *     key is 'y', then return TRUE to the caller. Otherwise, return
 *     FALSE;
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: TRUE if the 'y' key was pressed, otherwise FALSE
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 prompt_yesno(void)
{
	UNS_8 key [2];
	BOOL_32 yes = FALSE;

	term_dat_out(yesno_msg);
	key [1] = '\0';
	while (term_dat_in_ready() == 0);
	term_dat_in(key, 1);
	str_upper_to_lower(key);
	if (key [0] == 'y') 
	{
		yes = TRUE;
	}

	return yes;
}
