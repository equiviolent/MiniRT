# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hsabir <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/22 13:45:33 by hsabir            #+#    #+#              #
#    Updated: 2022/01/22 14:52:00 by hsabir           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



SHELL = /bin/sh

CFLAGS := ${CFLAGS}

CC     ?= gcc
LD     ?= gcc

INC_FLAGS := -Ilibs/libft -Ilibs/get_next_line/includes
LIBS := -Llibs/libft -Llibs/get_next_line/dist -lft -lmlx -lgnl

UNAME = $(shell uname -s)
ifeq ($(UNAME), Linux)
	NPROC := $(shell nproc)
	LIBS += -lmlx -lXext -lX11 -lm -lbsd
else
	NPROC := $(shell sysctl -n hw.ncpu)
	INC_FLAGS += -Ilibs/mlx
    LIBS += -Llibs/mlx -framework OpenGL -framework Appkit
endif

MAKEFLAGS += --output-sync=target
MAKEFLAGS += --no-print-directory

NAME ?= miniRT

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INCLUDE_DIR ?= ./includes

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(INCLUDE_DIR) -type d)
INC_FLAGS += $(addprefix -I,$(INC_DIRS))

LIB    := libs/libft/libft.a libs/get_next_line/dist/libgnl.a

CFLAGS += -Wall -Wextra -Werror
#CFLAGS += -O2 -march=native
#CFLAGS += -g3 -fsanitize=address -v

all:
	@$(MAKE) -j$(NPROC) $(NAME)

$(NAME): $(LIB) $(OBJS)
	@echo Linking $@
	@$(CC) $(CFLAGS) $(INC_FLAGS) $(OBJS) $(LIBS) -o $(NAME)

$(BUILD_DIR)/%.c.o: %.c
	@echo Compiling $@
	@mkdir -p $(dir $@)
	@$(CC) -c  $(CFLAGS) $(INC_FLAGS) $< -o $@

$(LIB):
	@$(MAKE) -C libs/libft
	@echo Libft done
	@$(MAKE) -C libs/get_next_line
	@echo GNL done

clean:
	@rm -rf $(BUILD_DIR)
	@$(MAKE) -C libs/libft clean
	@$(MAKE) -C libs/get_next_line clean
	@echo Clean done

fclean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(NAME)
	@$(MAKE) -C libs/libft fclean
	@$(MAKE) -C libs/get_next_line fclean
	@echo Fclean done

re: fclean
	@$(MAKE) -j$(NPROC) $(NAME)

.PHONY: all clean fclean re
