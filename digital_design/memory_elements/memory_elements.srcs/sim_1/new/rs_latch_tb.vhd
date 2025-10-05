----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 04:06:56 PM
-- Design Name: 
-- Module Name: rs_latch_tb - Behavioral
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

entity rs_latch_tb is
--  Port ( );
end rs_latch_tb;

architecture Behavioral of rs_latch_tb is

component rs_latch_struct is
Port ( S : in STD_LOGIC;
       R : in STD_LOGIC;
       Q : out STD_LOGIC;
       nQ : out STD_LOGIC);
end component;

signal S, R, Q, nQ : STD_LOGIC := '0';

begin

DUT: rs_latch_struct
    port map (
        S => S,
        R => R,
        Q => Q,
        nQ => nQ
    );

    stim_proc: process
    begin
        S <= '0'; R <= '0'; wait for 20 ns;

        S <= '1'; R <= '0'; wait for 20 ns;
        S <= '0'; R <= '0'; wait for 20 ns;

        S <= '0'; R <= '1'; wait for 20 ns;
        S <= '0'; R <= '0'; wait for 20 ns;

        S <= '1'; R <= '1'; wait for 20 ns;
        S <= '0'; R <= '0'; wait for 20 ns;

        assert false report "Simulation finished" severity failure;
    end process;

end Behavioral;
