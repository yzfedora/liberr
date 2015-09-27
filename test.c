/* This is a general error process library for linux in C
 *   		Copyright (C) 2015  Yang Zhang <yzfedora@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err_handler.h>

int main(void)
{
	pid_t pid;
	int status;

	/* For developer to print more message for debug. */
	err_setdebug(true);

	/* You can using journal -n 50 to display the syslog message. */
	err_setdaemon(true);
	err_dbg("This is debug test");
	err_msg("process %ld start to execute wiat() without any child",
			(long)getpid());
	if ((pid = wait(&status)) == -1)
		err_exit("wait");
	return 0;
}
