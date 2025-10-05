----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 12:11:55 PM
-- Design Name: 
-- Module Name: rs_latch_struct - Structural
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

entity rs_latch_struct is
    Port ( S : in STD_LOGIC;
           R : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end rs_latch_struct;

architecture Structural of rs_latch_struct is

signal q_int, nq_int : STD_LOGIC;
    
component nor2_beh is
    Port ( a : in STD_LOGIC;
           b : in STD_LOGIC;
           Q : out STD_LOGIC);
end component;

begin
    NOR1: nor2_beh port map (
        A => R,
        B => nq_int,
        Q => q_int
    );
    NOR2: nor2_beh port map (
        A => S,
        B => q_int,
        Q => nq_int
    );
    Q <= q_int;
    nQ <= nq_int;
end Structural;
