----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 05:00:33 PM
-- Design Name: 
-- Module Name: d_trigger_perm - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity d_trigger_perm is
    Port ( D : in STD_LOGIC;
           PERM : in STD_LOGIC;
           CLK : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end d_trigger_perm;

architecture Behavioral of d_trigger_perm is

signal state : STD_LOGIC := '0';

begin
    process(CLK, PERM)
    begin
        if rising_edge(CLK) then
            if PERM = '1' then
                state <= D;
            end if;
        end if;
    end process;
    Q <= state;
    nQ <= not state;
end Behavioral;
